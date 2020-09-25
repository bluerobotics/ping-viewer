#include <functional>

#include <QDebug>
#include <QDir>
#include <QLoggingCategory>
#include <QUrl>

#include "filelink.h"
#include "logger.h"
#include "settingsmanager.h"

PING_LOGGING_CATEGORY(PING_PROTOCOL_FILELINK, "ping.protocol.filelink")

FileLink::FileLink(QObject* parent)
    : AbstractLink("FileLink", parent)
    , _openModeFlag(QIODevice::ReadWrite)
    , _timer()
    , _inout(&_file)
    , _processLog(nullptr)
{
    _timer.start();
    setType(LinkType::File);
    connect(this, &AbstractLink::sendData, this, &FileLink::writeData);
}

void FileLink::writeData(const QByteArray& data)
{
    // Check if we have already opened the file
    if (!_file.isOpen()) {
        qCDebug(PING_PROTOCOL_FILELINK) << "File will be opened.";
        if (!_file.open(QIODevice::ReadWrite)) {
            qCDebug(PING_PROTOCOL_FILELINK) << "File was not open.";
            return;
        }

        // Create log header
        _inout << _logSensorStruct;
    }

    // This save the data as a structure to deal with the timestamp
    if (_openModeFlag == QIODevice::WriteOnly && _file.isWritable()) {
        QString time = QTime::fromMSecsSinceStartOfDay(_timer.elapsed()).toString(_timeFormat);
        Pack pack {time, data};
        _inout << pack.time << pack.data;
    } else {
        qCWarning(PING_PROTOCOL_FILELINK) << "Something is wrong!";
        qCDebug(PING_PROTOCOL_FILELINK) << "File is opened as write only:" << (_openModeFlag == QIODevice::WriteOnly);
        qCDebug(PING_PROTOCOL_FILELINK) << "File can be writable:" << _file.isWritable();
    }
}

bool FileLink::setConfiguration(const LinkConfiguration& linkConfiguration)
{
    _linkConfiguration = linkConfiguration;
    qCDebug(PING_PROTOCOL_FILELINK) << linkConfiguration;
    if (!linkConfiguration.isValid()) {
        qCDebug(PING_PROTOCOL_FILELINK) << LinkConfiguration::errorToString(linkConfiguration.error());
        return false;
    }

    setName(linkConfiguration.name());

    // Read or create the log ?
    // This flag does not change how the file will be open (ReadWrite)
    _openModeFlag = linkConfiguration.args()->at(1)[0] == 'r' ? QIODevice::ReadOnly : QIODevice::WriteOnly;

    _file.setFileName(linkConfiguration.args()->at(0));

    if (_openModeFlag == QIODevice::WriteOnly) {
        // The file will be created when something is received
        // Avoiding empty files
        // Check if path is writable
        return QFileInfo(QFileInfo(_file).canonicalPath()).isWritable();
    }

    // Everything after this point is to deal with reading data
    bool ok = _file.open(QIODevice::ReadWrite);
    if (ok) {
        LogSensorStruct logSensorStruct;
        // Check if file is valid
        _inout >> logSensorStruct;
        if (!logSensorStruct.isValid()) {
            qCWarning(PING_PROTOCOL_FILELINK) << "Log file does not contain a valid header.";
            qCDebug(PING_PROTOCOL_FILELINK) << logSensorStruct;
            // Close file since it's not valid
            _file.close();
            return false;
        }

        // Update internal struct
        _logSensorStruct = logSensorStruct;
        qCDebug(PING_PROTOCOL_FILELINK) << "Valid log file.";
        qCDebug(PING_PROTOCOL_FILELINK) << _logSensorStruct;
    } else {
        qCWarning(PING_PROTOCOL_FILELINK) << "It's not possible to open the file.";
    }
    return ok;
}

bool FileLink::startConnection()
{
    // The file will be created when something is received
    // Avoiding empty files
    if (_openModeFlag == QIODevice::WriteOnly) {
        return QFileInfo(QFileInfo(_file).canonicalPath()).isWritable();
    }

    if (!isOpen()) {
        return false;
    }

    _processLog.reset(new ProcessLog());
    _processLog->moveToThread(&_processLogThread);

    auto updateProcessReplayTime = [this]() {
        int replayTimeMs = SettingsManager::self()->realTimeReplay() ? 0 : 100;
        qCDebug(PING_PROTOCOL_FILELINK) << "Update replay time:" << replayTimeMs;
        _processLog.get()->setReplayTimeMs(replayTimeMs);
    };

    updateProcessReplayTime();

    connect(&_processLogThread, &QThread::started, _processLog.get(), &ProcessLog::run);
    connect(&_processLogThread, &QThread::finished, _processLog.get(), &ProcessLog::stop);
    connect(_processLog.get(), &ProcessLog::newPackage, this, &FileLink::newData);
    connect(_processLog.get(), &ProcessLog::packageIndexChanged, this, &FileLink::packageIndexChanged);
    connect(_processLog.get(), &ProcessLog::packageIndexChanged, this, &FileLink::elapsedTimeChanged);
    connect(SettingsManager::self(), &SettingsManager::realTimeReplayChanged, this,
        [updateProcessReplayTime] { updateProcessReplayTime(); });

    processFile();
    return true;
};

void FileLink::processFile()
{
    Pack pack;

    while (!_inout.atEnd()) {
        // Get data
        _inout >> pack.time >> pack.data;

        // Check if we have a new package
        if (pack.time.isEmpty()) {
            qCDebug(PING_PROTOCOL_FILELINK) << "No more packages !";
            break;
        }

        auto time = QTime::fromString(pack.time, _timeFormat);
        _processLog->append(time, pack.data);
    }
    _processLogThread.start();
    emit elapsedTimeChanged();
    emit totalTimeChanged();
}

LogSensorStruct FileLink::staticLogSensorStruct(const LinkConfiguration& linkConfiguration)
{
    // Check if configuration is valid
    auto openModeFlag = linkConfiguration.args()->at(1)[0] == 'r' ? QIODevice::ReadOnly : QIODevice::WriteOnly;
    if (openModeFlag == QIODevice::WriteOnly) {
        qCWarning(PING_PROTOCOL_FILELINK) << "Can't get LogSensorStruct from WriteOnly file.";
        return {};
    }

    // Check if file is valid
    QFile file(linkConfiguration.args()->at(0));
    bool ok = file.open(openModeFlag);
    if (!ok) {
        qCWarning(PING_PROTOCOL_FILELINK) << "Can't open file.";
        return {};
    }

    // Extract log header from file
    QDataStream in(&file);
    LogSensorStruct logSensorStruct;
    in >> logSensorStruct;
    file.close();

    return logSensorStruct;
}

bool FileLink::isOpen()
{
    // If filelink exist to create a log, the file will be only created after receiving the first data
    // To return at least a good answer, we do check the path to see if it's writable
    return (QFileInfo(QFileInfo(_file).canonicalPath()).isWritable() && _openModeFlag == QIODevice::WriteOnly)
        || _file.isReadable(); // If file is readable it's already opened and working
};

bool FileLink::finishConnection()
{
    // Stop reading log process if it's running
    if (_processLogThread.isRunning()) {
        _processLog->stop();
        _processLogThread.quit();
        _processLogThread.wait();
    }

    // Only close files that are open
    if (_file.isOpen()) {
        _file.close();
    }
    return true;
}

FileLink::~FileLink() { finishConnection(); }
