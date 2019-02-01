#include "logger.h"
#include "flasher.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>
#include <QProcess>
#include <QRegularExpression>
#include <QSerialPortInfo>
#include <QThread>

PING_LOGGING_CATEGORY(FLASH, "ping.flash")

Flasher::Flasher(QObject* parent)
    : QObject(parent)
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
    if(!_validBaudRates.contains(baudRate)) {
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
    if(!fileInfo.exists()) {
        qCCritical(FLASH) << "Firmware file does not exist:" << firmwareFilePath;
        return false;
    }

    _firmwareFilePath = fileInfo.absoluteFilePath();
    return true;
};

bool Flasher::setLink(const LinkConfiguration& link)
{
    if(!link.checkType(LinkType::Serial)) {
        qCCritical(FLASH) << "Link configuration is not valid:" << link;
        return false;
    }
    _link = link;
    return true;
};

void Flasher::setVerify(bool verify)
{
    _verify = verify;
};

QString Flasher::stm32flashPath()
{
#ifdef Q_OS_OSX
    // macdeployqt file do not put stm32flash binary in the same folder of pingviewer
    static QString absoluteBinPath = QCoreApplication::applicationDirPath() + "/../..";
#else
    static QString absoluteBinPath = QCoreApplication::applicationDirPath();
#endif
#ifdef Q_OS_WIN
    return absoluteBinPath + "/stm32flash.exe";
#else
    return absoluteBinPath + "/stm32flash";
#endif
}

void Flasher::flash()
{
    if(!QFile::exists(stm32flashPath())) {
        qCWarning(FLASH) << "stm32flash is not available! Flash procedure will abort.";
        qCWarning(FLASH) << "Searching in: " << stm32flashPath();
        return;
    }

    QSerialPortInfo pInfo(_link.serialPort());
    auto portLocation = pInfo.systemLocation();

    auto baudRate = QString::number(_baudRate);
    auto verifyArgument = _verify ? QStringLiteral("-v") : QString();

    QFileInfo firmwareFileInfo(_firmwareFilePath);
    if(!firmwareFileInfo.exists()) {
        auto errorMsg = QStringLiteral("Firmware file does not exist: %1").arg(_firmwareFilePath);
        qCCritical(FLASH) << errorMsg;
        setError(errorMsg);
        return;
    }

    static QString cmd = QStringLiteral("\"%0\" -w \"%1\" %2 -g 0x0 -b %3 %4").arg(
                             stm32flashPath(),
                             firmwareFileInfo.absoluteFilePath(),
                             _verify ? "-v" : "",
                             baudRate,
                             portLocation
                         );

    _firmwareProcess = QSharedPointer<QProcess>(new QProcess);
    _firmwareProcess->setEnvironment(QProcess::systemEnvironment());
    _firmwareProcess->setProcessChannelMode(QProcess::MergedChannels);
    qCDebug(FLASH) << "3... 2... 1...";
    qCDebug(FLASH) << cmd;
    _firmwareProcess->start(cmd);
    emit flashProgress(0);
    connect(_firmwareProcess.data(), &QProcess::readyReadStandardOutput, this, &Flasher::firmwareUpdatePercentage);
}

void Flasher::firmwareUpdatePercentage()
{
    QString output(_firmwareProcess->readAllStandardOutput());
    // Track values like: (12.23%)
    QRegularExpression regex("\\d{1,3}[.]\\d\\d");
    QRegularExpressionMatch match = regex.match(output);
    if(match.hasMatch()) {
        QStringList percs = match.capturedTexts();
        for(const auto& perc : percs) {
            float _fw_update_perc = perc.toFloat();
            qCDebug(FLASH) << _fw_update_perc;
            if (_fw_update_perc > 99.99) {
                QThread::msleep(1000);
                emit flashStateChanged(FlashFinished);
            } else {
                emit flashProgress(_fw_update_perc);
            }
        }
    }

    qCDebug(FLASH) << output;
}

void Flasher::setError(const QString& errorMessage)
{
    /* Update error before flashState
     *  This will avoid any problem related to error detection from state change
     */
    _error = errorMessage;
    emit errorChanged();
    flashStateChanged(Error);
}
