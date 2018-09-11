#include <functional>

#include <QDebug>
#include <QDir>
#include <QLoggingCategory>
#include <QTimer>
#include <QUrl>

#include "filelink.h"

Q_LOGGING_CATEGORY(PING_PROTOCOL_FILELINK, "ping.protocol.filelink")

FileLink::FileLink(QObject* parent)
    : AbstractLink(parent)
    , _openModeFlag(QIODevice::ReadWrite)
    , _time(QTime::currentTime())
    , _inout(&_file)
    , _logThread(nullptr)
{
    setType(LinkType::File);

    connect(this, &AbstractLink::sendData, this, &FileLink::_writeData);
}

void FileLink::_writeData(const QByteArray& data)
{
    // Check if we have already opened the file
    if(!_file.isOpen()) {
        qCDebug(PING_PROTOCOL_FILELINK) << "File will be opened.";
        if(!_file.open(QIODevice::ReadWrite)){
            qCDebug(PING_PROTOCOL_FILELINK) << "File was not open.";
            return;
        }
    }

    // This save the data as a structure to deal with the timestamp
    if(_openModeFlag == QIODevice::WriteOnly && _file.isWritable()) {
        QString time = _time.currentTime().toString(_timeFormat);
        Pack pack{time, data};
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
    qCDebug(PING_PROTOCOL_FILELINK) << linkConfiguration.toString();
    if(!linkConfiguration.isValid()) {
        qCDebug(PING_PROTOCOL_FILELINK) << LinkConfiguration::errorToString(linkConfiguration.error());
        return false;
    }

    setName(linkConfiguration.name());

    // Read or create the log ?
    // This flag does not change how the file will be open (ReadWrite)
    _openModeFlag = linkConfiguration.args()->at(1)[0] == "r" ? QIODevice::ReadOnly : QIODevice::WriteOnly;

    _file.setFileName(linkConfiguration.args()->at(0));

    return true;
}

bool FileLink::startConnection() {
    // WriteOnly is used only to save data
    if(_openModeFlag == QIODevice::WriteOnly) {
        // The file will be created when something is received
        // Avoiding empty files
        // Check if path is writable
        return QFileInfo(QFileInfo(_file).canonicalPath()).isWritable();
    }

    // Everything after this point is to deal with reading data
    bool ok = _file.open(QIODevice::ReadWrite);
    if(ok) {
        Pack pack;
        if(_logThread) {
            // Disconnect LogThread
            disconnect(_logThread.get(), &LogThread::newPackage, this, &FileLink::newData);
            disconnect(_logThread.get(), &LogThread::packageIndexChanged, this, &FileLink::packageIndexChanged);
            disconnect(_logThread.get(), &LogThread::packageIndexChanged, this, &FileLink::elapsedTimeChanged);
        }
        _logThread.reset(new LogThread());
        connect(_logThread.get(), &LogThread::newPackage, this, &FileLink::newData);
        connect(_logThread.get(), &LogThread::packageIndexChanged, this, &FileLink::packageIndexChanged);
        connect(_logThread.get(), &LogThread::packageIndexChanged, this, &FileLink::elapsedTimeChanged);
        while(true) {
            // Get data
            _inout >> pack.time >> pack.data;

            // Check if we have a new package
            if(pack.time.isEmpty()) {
                qCDebug(PING_PROTOCOL_FILELINK) << "No more packages !";
                break;
            }

            QTime time = QTime::fromString(pack.time, _timeFormat);
            _logThread->append(time, pack.data);
        }
        _logThread->start();
        emit elapsedTimeChanged();
        emit totalTimeChanged();
    }
    return ok;
};

bool FileLink::isOpen() {
    // If filelink exist to create a log, the file will be only created after receiving the first data
    // To return at least a good answer, we do check the path to see if it's writable
    return (QFileInfo(QFileInfo(_file).canonicalPath()).isWritable() && _openModeFlag == QIODevice::WriteOnly)
        || _file.isReadable(); // If file is readable it's already opened and working
};

bool FileLink::finishConnection()
{
    // Only close files that are open
    if(_file.isOpen()) {
        _file.close();
    }
    return true;
}

FileLink::~FileLink()
{
}
