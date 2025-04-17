#include "flasher.h"
#include "logger.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>
#include <QSerialPortInfo>
#include <QStandardPaths>
#include <QThread>

PING_LOGGING_CATEGORY(FLASH, "ping.flash")

Flasher::Flasher(QObject* parent, const QList<QVariant> validBaudRates)
    : QObject(parent)
    , _validBaudRates(validBaudRates)
{
    _binRelativePath =
#ifdef Q_OS_OSX
        // macdeployqt file do not put stm32flash binary in the same folder of pingviewer
        QCoreApplication::applicationDirPath() + "/../..";
#else
        QCoreApplication::applicationDirPath();
#endif
}

bool Flasher::setBaudRate(int baudRate)
{
    if (!_validBaudRates.contains(baudRate)) {
        qCCritical(FLASH) << "Invalid baud rate:" << baudRate;
        qCCritical(FLASH) << "Valid baud rates are:" << _validBaudRates;
        return false;
    }

    _baudRate = baudRate;
    return true;
};

bool Flasher::setFirmwarePath(const QString& firmwareFilePath)
{
    QFileInfo fileInfo(firmwareFilePath);
    if (!fileInfo.exists()) {
        qCCritical(FLASH) << "Firmware file does not exist:" << firmwareFilePath;
        return false;
    }

    _firmwareFilePath = fileInfo.absoluteFilePath();
    return true;
};

bool Flasher::setLink(const LinkConfiguration& link)
{
    if (!link.checkType(LinkType::Serial)) {
        qCCritical(FLASH) << "Link configuration is not valid:" << link;
        return false;
    }
    _link = link;
    return true;
};

void Flasher::setVerify(bool verify) { _verify = verify; };

QString Flasher::stm32flashPath()
{
    static const QString binaryName =
#ifdef Q_OS_WIN
        "stm32flash.exe";
#else
        "stm32flash";
#endif

#ifdef Q_OS_OSX
    // macdeployqt file do not put stm32flash binary in the same folder of pingviewer
    static const QString absoluteBinPath = QCoreApplication::applicationDirPath() + "/../..";
#else
    static const QString absoluteBinPath = QCoreApplication::applicationDirPath();
#endif

    const QString localPathString = absoluteBinPath + "/" + binaryName;
    if (QFile::exists(localPathString)) {
        qCDebug(FLASH) << "Found program in:" << localPathString;
        return localPathString;
    }

    // Check if program is installed
    QString absoluteSystemBinPath = QStandardPaths::findExecutable(binaryName);
    if (!absoluteSystemBinPath.isEmpty()) {
        qCDebug(FLASH) << "Found program in:" << absoluteSystemBinPath;
    }
    return absoluteSystemBinPath;
}

void Flasher::flash()
{
    if (!QFile::exists(stm32flashPath())) {
        QString output = QStringLiteral("stm32flash is not available! Flash procedure will abort.");
        qCCritical(FLASH) << "Searching in: " << stm32flashPath();
        qCCritical(FLASH) << output;
        setState(Error, output);

        return;
    }

    QSerialPortInfo pInfo(_link.serialPort());
    auto portLocation = pInfo.systemLocation();

    auto baudRate = QString::number(_baudRate);
    auto verifyArgument = _verify ? QStringLiteral("-v") : QString();

    QFileInfo firmwareFileInfo(_firmwareFilePath);
    if (!firmwareFileInfo.exists()) {
        auto errorMsg = QStringLiteral("Firmware file does not exist: %1").arg(_firmwareFilePath);
        qCCritical(FLASH) << errorMsg;
        setState(Error, errorMsg);
        return;
    }

    QStringList cmd {stm32flashPath(), "-w", firmwareFileInfo.absoluteFilePath(), _verify ? "-v" : "", "-g", "0x0",
        "-b", baudRate, portLocation};

    _firmwareProcess = QSharedPointer<QProcess>(new QProcess);

    connect(_firmwareProcess.data(), &QProcess::readyReadStandardOutput, this, [this] {
        // Error strings used to detect important messages for the user
        static const QStringList errorStrings = {
            "error",
            "fail",
            "fatal",
            "invalid",
            "unexpected",
        };
        QString output(_firmwareProcess->readAllStandardOutput());
        for (const auto& errorString : errorStrings) {
            if (output.contains(errorString, Qt::CaseInsensitive)) {
                qCCritical(FLASH) << output;
                setState(Error, output);
                // Break is necessary to avoid messages with multiple keys like:
                // Error number... Fatal behaviour.. Unexpected port.
                break;
            }
        }

        firmwareUpdatePercentage(output);
    });

    connect(_firmwareProcess.data(), &QProcess::errorOccurred, this, [this](QProcess::ProcessError error) {
        QString message = QStringLiteral("Unexpected error in process (%1): %2")
                              .arg(QString::number(error), _firmwareProcess->errorString());
        qCCritical(FLASH) << message;
        setState(Error, message);
    });

    connect(_firmwareProcess.data(), qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this,
        [](int exitCode, QProcess::ExitStatus exitStatus) {
            if (exitCode == 0) {
                return;
            }

            // It's not necessary to use setMessage here, since this signal is a result of the process
            // and not from stm32flash
            static QString message;
            if (exitStatus == QProcess::NormalExit) {
                message = QStringLiteral("Process exited normally with exit code: %1").arg(exitCode);
                qCWarning(FLASH) << message;
            } else {
                message = QStringLiteral("Process crashed with exit code: %1").arg(exitCode);
                qCCritical(FLASH) << message;
            }
        });

    connect(_firmwareProcess.data(), &QProcess::stateChanged, this,
        [this](QProcess::ProcessState state) { qCDebug(FLASH) << "New process state:" << state; });

    _firmwareProcess->setEnvironment(QProcess::systemEnvironment());
    _firmwareProcess->setProcessChannelMode(QProcess::MergedChannels);
    qCDebug(FLASH) << "3... 2... 1...";
    qCDebug(FLASH) << cmd;
    auto program = cmd.takeFirst();
    _firmwareProcess->start(program, cmd);
    emit flashProgress(0);
}

void Flasher::firmwareUpdatePercentage(const QString& output)
{
    // Track values like: (12.23%)
    QRegularExpression regex("\\d{1,3}[.]\\d\\d");
    QRegularExpressionMatch match = regex.match(output);
    if (match.hasMatch()) {
        QStringList percs = match.capturedTexts();
        for (const auto& perc : percs) {
            float _fw_update_perc = perc.toFloat();
            qCDebug(FLASH) << _fw_update_perc;
            if (_fw_update_perc > 99.99) {
                QThread::msleep(1000);
                setState(FlashFinished);
            } else {
                emit flashProgress(_fw_update_perc);
            }
        }
    }

    qCDebug(FLASH) << output;
}

void Flasher::setState(Flasher::States state, QString message)
{
    if (_state != state) {
        _state = state;
        emit stateChanged(state);
    }

    setMessage(message);
}

void Flasher::setMessage(const QString& message)
{
    auto msg = message.trimmed();
    if (msg != _message) {
        _message = msg;
        emit messageChanged();
    }
}
