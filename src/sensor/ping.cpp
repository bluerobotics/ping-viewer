#include "ping.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>
#include <QThread>
#include <QUrl>

#include "link/seriallink.h"
#include "pingmessage/pingmessage.h"
#include "pingmessage/pingmessage_ping1D.h"

Q_LOGGING_CATEGORY(PING_PROTOCOL_PING, "ping.protocol.ping")

Ping::Ping() : Sensor()
{
    _points.reserve(_num_points);
    for (int i = 0; i < _num_points; i++) {
        _points.append(0);
    }
    _parser = new PingParser();
    connect(dynamic_cast<PingParser*>(_parser), &PingParser::newMessage, this, &Ping::handleMessage);
    connect(link(), &AbstractLink::newData, _parser, &Parser::parseBuffer);
    emit linkUpdate();

    _requestTimer.setInterval(1000);
    connect(&_requestTimer, &QTimer::timeout, this, [this] { request(PingMessage::ping1D_profile); });

    //connectLink("2:/dev/ttyUSB2:115200");

    connect(&_detector, &ProtocolDetector::_detected, this, &Ping::connectLink);
    _detector.start();

    connect(this, &Ping::autoDetectUpdate, this, [this](bool autodetect) {
        if(!autodetect) {
            if(_detector.isRunning()) {
                _detector.exit();
            }
        } else {
            if(!_detector.isRunning()) {
                _detector.start();
            }
        }
    });
}

void Ping::connectLink(const QString& connString)
{
    if(_detector.isRunning()) {
        _detector.exit();
    }
    setAutoDetect(false);
    Sensor::connectLink(connString);
}

void Ping::handleMessage(PingMessage msg)
{
    qCDebug(PING_PROTOCOL_PING) << "Handling Message:" << msg.message_id() << "Checksum Pass:" << msg.verifyChecksum();

    switch (msg.message_id()) {

    case PingMessage::ping1D_fw_version: {
        ping_msg_ping1D_fw_version m(msg);
        _device_type = m.device_type();
        _device_model = m.device_model();
        _fw_version_major = m.fw_version_major();
        _fw_version_minor = m.fw_version_minor();

        emit deviceTypeUpdate();
        emit deviceModelUpdate();
        emit fwVersionMajorUpdate();
        emit fwVersionMinorUpdate();
    }
    break;

    case PingMessage::ping1D_distance: {
        ping_msg_ping1D_distance m(msg);
        _distance = m.distance();
        _confidence = m.confidence();
        _pulse_usec = m.pulse_usec();
        _ping_number = m.ping_number();
        _start_mm = m.start_mm();
        _length_mm = m.length_mm();
        _gain_index = m.gain_index();

        // TODO, change to distMsgUpdate() or similar
        emit distanceUpdate();
        emit pingNumberUpdate();
        emit confidenceUpdate();
        emit pulseUsecUpdate();
        emit startMmUpdate();
        emit lengthMmUpdate();
        emit gainIndexUpdate();
    }
    break;

    case PingMessage::ping1D_profile: {
        ping_msg_ping1D_profile m(msg);
        _distance = m.distance();
        _confidence = m.confidence();
        _pulse_usec = m.pulse_usec();
        _ping_number = m.ping_number();
        _start_mm = m.start_mm();
        _length_mm = m.length_mm();
        _gain_index = m.gain_index();
//        num_points = m.num_points(); // const
//        memcpy(_points.data(), m.data(), _num_points); // careful with constant

        // This is necessary to convert <uint8_t> to <int>
        // QProperty only supports vector<int>, otherwise, we could use memcpy
        for (int i = 0; i < m.num_points(); i++) {
            _points.replace(i, m.data()[i] / 255.0); // TODO we should really be working in ints
        }

        // TODO, change to distMsgUpdate() or similar
        emit distanceUpdate();
        emit pingNumberUpdate();
        emit confidenceUpdate();
        emit pulseUsecUpdate();
        emit startMmUpdate();
        emit lengthMmUpdate();
        emit gainIndexUpdate();
        emit pointsUpdate();
    }
    break;

    case PingMessage::ping1D_mode: {
        ping_msg_ping1D_mode m(msg);
        _mode_auto = m.auto_manual();
        emit modeAutoUpdate();
    }
    break;

    case PingMessage::ping1D_ping_rate_msec: {
        ping_msg_ping1D_ping_rate_msec m(msg);
        _msec_per_ping = m.msec_per_ping();
        emit msecPerPingUpdate();
    }
    break;

    default:
        qCritical() << "UNHANDLED MESSAGE ID:" << msg.message_id();
        break;
    }

    _dstId = msg.dst_device_id();
    _srcId = msg.src_device_id();

    emit dstIdUpdate();
    emit srcIdUpdate();

//    printStatus();
}

void Ping::firmwareUpdate(QString fileUrl, bool sendPingGotoBootloader)
{
    SerialLink* serialLink = dynamic_cast<SerialLink*>(link());

    if (!serialLink) {
        return;
    }

    if(!link()->isOpen()) {
        return;
    }

    setPollFrequency(0);

    if (sendPingGotoBootloader) {
        qCDebug(PING_PROTOCOL_PING) << "Put it in bootloader mode.";
        ping_msg_ping1D_goto_bootloader m;
        m.updateChecksum();
        link()->sendData(QByteArray(reinterpret_cast<const char*>(m.msgData.data()), m.msgData.size()));
    }

    // Wait for bytes to be written before finishing the connection
    while (serialLink->QSerialPort::bytesToWrite()) {
        qCDebug(PING_PROTOCOL_PING) << "Waiting for bytes to be written...";
        serialLink->QSerialPort::waitForBytesWritten();
        qCDebug(PING_PROTOCOL_PING) << "Done !";
    }

    qCDebug(PING_PROTOCOL_PING) << "Finish connection.";
    // TODO: Move thread delay to something more.. correct.
    QThread::usleep(500e3);
    link()->finishConnection();


    QSerialPortInfo pInfo(serialLink->QSerialPort::portName());
    QString portLocation = pInfo.systemLocation();

    qCDebug(PING_PROTOCOL_PING) << "Start flash.";
    QThread::usleep(500e3);
    flash(portLocation, QUrl(fileUrl).toLocalFile());
}

void Ping::flash(const QString& portLocation, const QString& firmwareFile)
{
#ifdef Q_OS_OSX
    // macdeployqt file do not put stm32flash binary in the same folder of pingviewer
    static QString binPath = QCoreApplication::applicationDirPath() + "/../..";
#else
    static QString binPath = QCoreApplication::applicationDirPath();
#endif
    static QString cmd = binPath + "/stm32flash -w %0 %1 -v -g 0x0";

    _firmwareProcess = QSharedPointer<QProcess>(new QProcess);
    _firmwareProcess->setEnvironment(QProcess::systemEnvironment());
    _firmwareProcess->setProcessChannelMode(QProcess::MergedChannels);
    qCDebug(PING_PROTOCOL_PING) << "3... 2... 1...";
    qCDebug(PING_PROTOCOL_PING) << cmd.arg(QFileInfo(firmwareFile).absoluteFilePath(), portLocation);
    _firmwareProcess->start(cmd.arg(QFileInfo(firmwareFile).absoluteFilePath(), portLocation));
    emit flashProgress(0);
    connect(_firmwareProcess.data(), &QProcess::readyReadStandardOutput, this, &Ping::firmwareUpdatePercentage);
}

void Ping::firmwareUpdatePercentage()
{
    QString output(_firmwareProcess->readAllStandardOutput());
    // Track values like: (12.23%)
    QRegularExpression regex("\\d{1,3}[.]\\d\\d");
    QRegularExpressionMatch match = regex.match(output);
    if(match.hasMatch()) {
        QStringList percs = match.capturedTexts();
        for(const auto& perc : percs) {
            _fw_update_perc = perc.toFloat();

            if (_fw_update_perc > 99.99) {
                emit flashComplete();
                QThread::usleep(500e3);
                _detector.start();
            } else {
                emit flashProgress(_fw_update_perc);
            }
        }
    }

    qCDebug(PING_PROTOCOL_PING) << output;
}

void Ping::request(int id)
{
    qCDebug(PING_PROTOCOL_PING) << "Requesting:" << id;

    ping_msg_ping1D_empty m;
    m.set_id(id);
    m.updateChecksum();
    writeMessage(m);
}

QVariant Ping::pollFrequency()
{
    if (!_requestTimer.isActive()) {
        return 0;
    }
    return 1000.0f / _requestTimer.interval();
}

void Ping::setPollFrequency(QVariant pollFrequency)
{
    if (pollFrequency.toInt() <= 0) {
        if (_requestTimer.isActive()) {
            _requestTimer.stop();
        }
    } else {
        int period_ms = 1000.0f / pollFrequency.toInt();
        qCDebug(PING_PROTOCOL_PING) << "setting f" << pollFrequency.toInt() << period_ms;
        _requestTimer.setInterval(period_ms);
        if (!_requestTimer.isActive()) {
            _requestTimer.start();
        }

        set_msec_per_ping(period_ms);
    }

    qCDebug(PING_PROTOCOL_PING) << "Poll period" << pollFrequency;
    emit pollFrequencyUpdate();
}

void Ping::printStatus()
{
    qCDebug(PING_PROTOCOL_PING) << "Ping Status:";
    qCDebug(PING_PROTOCOL_PING) << "\t- srcId:" << _srcId;
    qCDebug(PING_PROTOCOL_PING) << "\t- dstID:" << _dstId;
    qCDebug(PING_PROTOCOL_PING) << "\t- device_type:" << _device_type;
    qCDebug(PING_PROTOCOL_PING) << "\t- device_model:" << _device_model;
    qCDebug(PING_PROTOCOL_PING) << "\t- fw_version_major:" << _fw_version_major;
    qCDebug(PING_PROTOCOL_PING) << "\t- fw_version_minor:" << _fw_version_minor;
    qCDebug(PING_PROTOCOL_PING) << "\t- distance:" << _distance;
    qCDebug(PING_PROTOCOL_PING) << "\t- confidence:" << _confidence;
    qCDebug(PING_PROTOCOL_PING) << "\t- pulse_usec:" << _pulse_usec;
    qCDebug(PING_PROTOCOL_PING) << "\t- ping_number:" << _ping_number;
    qCDebug(PING_PROTOCOL_PING) << "\t- start_mm:" << _start_mm;
    qCDebug(PING_PROTOCOL_PING) << "\t- length_mm:" << _length_mm;
    qCDebug(PING_PROTOCOL_PING) << "\t- gain_index:" << _gain_index;
    qCDebug(PING_PROTOCOL_PING) << "\t- mode_auto:" << _mode_auto;
    qCDebug(PING_PROTOCOL_PING) << "\t- msec_per_ping:" << _msec_per_ping;
//    qCDebug(PING_PROTOCOL_PING) << "\t- points:" << QByteArray((const char*)points, num_points).toHex();
}

void Ping::writeMessage(const PingMessage &msg)
{
    if(link()) {
        if(link()->isOpen()) {
            // todo add link::write(char*, int size)
            link()->sendData(QByteArray(reinterpret_cast<const char*>(msg.msgData.data()), msg.msgData.size()));
        }
    }
}
