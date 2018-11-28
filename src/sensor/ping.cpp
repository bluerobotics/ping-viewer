#include "ping.h"

#include <functional>

#include <QCoreApplication>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QStringList>
#include <QThread>
#include <QUrl>

#include "link/seriallink.h"
#include "networktool.h"
#include "notificationmanager.h"
#include "settingsmanager.h"

Q_LOGGING_CATEGORY(PING_PROTOCOL_PING, "ping.protocol.ping")

const int Ping::_pingMaxFrequency = 50;

Ping::Ping() : Sensor()
{
    _points.reserve(_num_points);
    for (int i = 0; i < _num_points; i++) {
        _points.append(0);
    }
    _parser = new PingParser();
    connect(dynamic_cast<PingParser*>(_parser), &PingParser::newMessage, this, &Ping::handleMessage);
    connect(dynamic_cast<PingParser*>(_parser), &PingParser::parseError, this, &Ping::parserErrorsUpdate);
    connect(link(), &AbstractLink::newData, _parser, &Parser::parseBuffer);
    emit linkUpdate();

    _periodicRequestTimer.setInterval(1000);
    connect(&_periodicRequestTimer, &QTimer::timeout, this, [this] {
        if(!link()->isWritable())
        {
            qCWarning(PING_PROTOCOL_PING) << "Can't write in this type of link.";
            _periodicRequestTimer.stop();
            return;
        }

        if(!link()->isOpen())
        {
            qCCritical(PING_PROTOCOL_PING) << "Can't write, port is not open!";
            _periodicRequestTimer.stop();
            return;
        }

        // Update lost messages count
        _lostMessages = 0;
        for(const auto& requestedId : requestedIds)
        {
            _lostMessages += requestedId.waiting;
        }
        emit lostMessagesUpdate();

        request(Ping1DNamespace::Pcb_temperature);
        request(Ping1DNamespace::Processor_temperature);
        request(Ping1DNamespace::Voltage_5);
        request(Ping1DNamespace::Mode_auto);
    });

    //connectLink(LinkType::Serial, {"/dev/ttyUSB2", "115200"});

    connect(detector(), &ProtocolDetector::connectionDetected, this, [this] {
        qCDebug(PING_PROTOCOL_PING) << "New device detected";
        setAutoDetect(false);

        // Request device information
        request(Ping1DNamespace::Mode_auto);
        request(Ping1DNamespace::Profile);
        request(Ping1DNamespace::Firmware_version);
        request(Ping1DNamespace::Device_id);
        request(Ping1DNamespace::Speed_of_sound);

        if(link()->isWritable())
        {
            // Save configuration
            SettingsManager::self()->lastLinkConfiguration(*link()->configuration());
            // Start periodic request timer
            _periodicRequestTimer.start();
        }
    });

    // Wait for device id to load the correct settings
    connect(this, &Ping::srcIdUpdate, this, &Ping::setLastPingConfiguration);

    connect(this, &Ping::firmwareVersionMinorUpdate, this, [this] {
        // Wait for firmware information to be available before looking for new versions
        static bool once = false;
        if(!once)
        {
            once = true;
            NetworkTool::self()->checkNewFirmware("ping1d", std::bind(&Ping::checkNewFirmwareInGitHubPayload, this,
                                                  std::placeholders::_1));
        }
    });

    connect(this, &Ping::autoDetectUpdate, this, [this](bool autodetect) {
        if(!autodetect) {
            if(detector()->isRunning()) {
                detector()->stop();
            }
        } else {
            if(!detector()->isRunning()) {
                detector()->scan();
            }
        }
    });

    // Load last successful connection
    auto config = SettingsManager::self()->lastLinkConfiguration();
    qCDebug(PING_PROTOCOL_PING) << "Loading last configuration connection from settings:" << config;
    addDetectionLink(config);
    detectorThread()->start();
}

void Ping::loadLastPingConfigurationSettings()
{
    // Set default values
    for(const auto& key : _pingConfiguration.keys()) {
        _pingConfiguration[key].value = _pingConfiguration[key].defaultValue;
    }

    // Load settings for device using device id
    QVariant pingConfigurationVariant = SettingsManager::self()->getMapValue({"Ping", "PingConfiguration", QString(_srcId)});
    if(pingConfigurationVariant.type() != QVariant::Map) {
        qCWarning(PING_PROTOCOL_PING) << "No valid PingConfiguration in settings." << pingConfigurationVariant.type();
        return;
    }

    // Get the value of each configuration and set it on device
    auto map = pingConfigurationVariant.toMap();
    for(const auto& key : _pingConfiguration.keys()) {
        _pingConfiguration[key].set(map[key]);
    }
}

void Ping::updatePingConfigurationSettings()
{
    // Save all sensor configurations
    for(const auto& key : _pingConfiguration.keys()) {
        auto& dataStruct = _pingConfiguration[key];
        dataStruct.set(dataStruct.getClassValue());
        SettingsManager::self()->setMapValue({"Ping", "PingConfiguration", QString(_srcId), key}, dataStruct.value);
    }
}

void Ping::addDetectionLink(const LinkConfiguration& linkConfiguration)
{
    if(linkConfiguration.isValid()) {
        detector()->appendConfiguration(linkConfiguration);
    } else {
        qCDebug(PING_PROTOCOL_PING) << "Invalid configuration:" << linkConfiguration.errorToString();
    }
}

void Ping::addDetectionLink(LinkType connType, const QStringList& connString)
{
    LinkConfiguration linkConfiguration{connType, connString};
    addDetectionLink(linkConfiguration);
}

void Ping::connectLink(LinkType connType, const QStringList& connString)
{
    Sensor::connectLink(LinkConfiguration{connType, connString});
    _periodicRequestTimer.start();
}

void Ping::handleMessage(PingMessage msg)
{
    qCDebug(PING_PROTOCOL_PING) << "Handling Message:" << msg.message_id() << "Checksum Pass:" << msg.verifyChecksum();

    auto& requestedId = requestedIds[static_cast<Ping1DNamespace::msg_ping1D_id>(msg.message_id())];
    if(requestedId.waiting) {
        requestedId.waiting--;
        requestedId.ack++;
    }

    switch (msg.message_id()) {

    case Ping1DNamespace::Ack: {
        qCDebug(PING_PROTOCOL_PING) << "Sensor ACK.";
        break;
    }

    case Ping1DNamespace::Nack: {
        ping_msg_ping1D_nack nackMessage{msg};
        qCCritical(PING_PROTOCOL_PING) << "Sensor NACK!";
        _nack_msg = QString("%1: %2").arg(nackMessage.nack_message()).arg(nackMessage.nacked_id());
        qCDebug(PING_PROTOCOL_PING) << "NACK message:" << _nack_msg;
        emit nackMsgUpdate();

        auto& nackRequestedId = requestedIds[static_cast<Ping1DNamespace::msg_ping1D_id>(nackMessage.nacked_id())];
        if(nackRequestedId.waiting) {
            nackRequestedId.waiting--;
            nackRequestedId.nack++;
        }
        break;
    }

    // needs dynamic-payload patch
    case Ping1DNamespace::Ascii_text: {
        _ascii_text = ping_msg_ping1D_ascii_text(msg).ascii_message();
        qCInfo(PING_PROTOCOL_PING) << "Sensor status:" << _ascii_text;
        emit asciiTextUpdate();
        break;
    }

    case Ping1DNamespace::Firmware_version: {
        ping_msg_ping1D_firmware_version m(msg);
        _device_type = m.device_type();
        _device_model = m.device_model();
        _firmware_version_major = m.firmware_version_major();
        _firmware_version_minor = m.firmware_version_minor();

        emit deviceTypeUpdate();
        emit deviceModelUpdate();
        emit firmwareVersionMajorUpdate();
        emit firmwareVersionMinorUpdate();
    }
    break;

    // This message is deprecated, it provides no added information because
    // the device id is already supplied in every message header
    case Ping1DNamespace::Device_id: {
        ping_msg_ping1D_device_id m(msg);
        _srcId = m.src_device_id();

        emit srcIdUpdate();
    }
    break;

    case Ping1DNamespace::Distance: {
        ping_msg_ping1D_distance m(msg);
        _distance = m.distance();
        _confidence = m.confidence();
        _pulse_duration = m.pulse_duration();
        _ping_number = m.ping_number();
        _scan_start = m.scan_start();
        _scan_length = m.scan_length();
        _gain_index = m.gain_index();

        // TODO, change to distMsgUpdate() or similar
        emit distanceUpdate();
        emit pingNumberUpdate();
        emit confidenceUpdate();
        emit pulseDurationUpdate();
        emit scanStartUpdate();
        emit scanLengthUpdate();
        emit gainIndexUpdate();
    }
    break;

    case Ping1DNamespace::Distance_simple: {
        ping_msg_ping1D_distance_simple m(msg);
        _distance = m.distance();
        _confidence = m.confidence();

        emit distanceUpdate();
        emit confidenceUpdate();
    }
    break;

    case Ping1DNamespace::Profile: {
        ping_msg_ping1D_profile m(msg);
        _distance = m.distance();
        _confidence = m.confidence();
        _pulse_duration = m.pulse_duration();
        _ping_number = m.ping_number();
        _scan_start = m.scan_start();
        _scan_length = m.scan_length();
        _gain_index = m.gain_index();
//        _num_points = m.profile_data_length(); // const for now
//        memcpy(_points.data(), m.data(), _num_points); // careful with constant

        // This is necessary to convert <uint8_t> to <int>
        // QProperty only supports vector<int>, otherwise, we could use memcpy, like the two lines above
        for (int i = 0; i < m.profile_data_length(); i++) {
            _points.replace(i, m.profile_data()[i] / 255.0);
        }

        // TODO, change to distMsgUpdate() or similar
        emit distanceUpdate();
        emit pingNumberUpdate();
        emit confidenceUpdate();
        emit pulseDurationUpdate();
        emit scanStartUpdate();
        emit scanLengthUpdate();
        emit gainIndexUpdate();
        emit pointsUpdate();
    }
    break;

    case Ping1DNamespace::Mode_auto: {
        ping_msg_ping1D_mode_auto m(msg);
        if(_mode_auto != m.mode_auto()) {
            _mode_auto = m.mode_auto();
            emit modeAutoUpdate();
        }
    }
    break;

    case Ping1DNamespace::Ping_interval: {
        ping_msg_ping1D_ping_interval m(msg);
        _ping_interval = m.ping_interval();
        emit pingIntervalUpdate();
    }
    break;

    case Ping1DNamespace::Range: {
        ping_msg_ping1D_range m(msg);
        _scan_start = m.scan_start();
        _scan_length = m.scan_length();
        emit scanLengthUpdate();
        emit scanStartUpdate();
    }
    break;

    case Ping1DNamespace::General_info: {
        ping_msg_ping1D_general_info m(msg);
        _gain_index = m.gain_index();
        emit gainIndexUpdate();
    }
    break;

    case Ping1DNamespace::Gain_index: {
        ping_msg_ping1D_gain_index m(msg);
        _gain_index = m.gain_index();
        emit gainIndexUpdate();
    }
    break;

    case Ping1DNamespace::Speed_of_sound: {
        ping_msg_ping1D_speed_of_sound m(msg);
        _speed_of_sound = m.speed_of_sound();
        emit speedOfSoundUpdate();
    }
    break;

    case Ping1DNamespace::Processor_temperature: {
        ping_msg_ping1D_processor_temperature m(msg);
        _processor_temperature = m.processor_temperature();
        emit processorTemperatureUpdate();
    }
    break;

    case Ping1DNamespace::Pcb_temperature: {
        ping_msg_ping1D_pcb_temperature m(msg);
        _pcb_temperature = m.pcb_temperature();
        emit pcbTemperatureUpdate();
    }
    break;

    case Ping1DNamespace::Voltage_5: {
        ping_msg_ping1D_voltage_5 m(msg);
        _board_voltage = m.voltage_5(); // millivolts
        emit boardVoltageUpdate();
    }
    break;

    default:
        qWarning(PING_PROTOCOL_PING) << "UNHANDLED MESSAGE ID:" << msg.message_id();
        break;
    }

    // TODO is this signalling expensive?
    // we can cut down on this a lot in general
    _dstId = msg.dst_device_id();
    _srcId = msg.src_device_id();

    emit dstIdUpdate();
    emit srcIdUpdate();
    emit parsedMsgsUpdate();

//    printStatus();
}

void Ping::firmwareUpdate(QString fileUrl, bool sendPingGotoBootloader, int baud, bool verify)
{
    SerialLink* serialLink = dynamic_cast<SerialLink*>(link());

    if (!serialLink) {
        return;
    }

    if(!link()->isOpen()) {
        return;
    }

    setPingFrequency(0);

    if (sendPingGotoBootloader) {
        qCDebug(PING_PROTOCOL_PING) << "Put it in bootloader mode.";
        ping_msg_ping1D_goto_bootloader m;
        m.updateChecksum();
        emit link()->sendData(QByteArray(reinterpret_cast<const char*>(m.msgData), m.msgDataLength()));
    }

    // Wait for bytes to be written before finishing the connection
    while (serialLink->port()->bytesToWrite()) {
        qCDebug(PING_PROTOCOL_PING) << "Waiting for bytes to be written...";
        serialLink->port()->waitForBytesWritten();
        qCDebug(PING_PROTOCOL_PING) << "Done !";
    }

    qCDebug(PING_PROTOCOL_PING) << "Finish connection.";
    // TODO: Move thread delay to something more.. correct.
    QThread::msleep(500);
    link()->finishConnection();


    QSerialPortInfo pInfo(serialLink->port()->portName());
    QString portLocation = pInfo.systemLocation();

    qCDebug(PING_PROTOCOL_PING) << "Start flash.";
    QThread::msleep(500);
    flash(portLocation, QUrl(fileUrl).toLocalFile(), baud, verify);
}

void Ping::flash(const QString& portLocation, const QString& firmwareFile, int baud, bool verify)
{
#ifdef Q_OS_OSX
    // macdeployqt file do not put stm32flash binary in the same folder of pingviewer
    static QString binPath = '"' + QCoreApplication::applicationDirPath() + "/../..";
#else
    static QString binPath = '"' + QCoreApplication::applicationDirPath();
#endif
    static QString cmd = binPath + QStringLiteral("/stm32flash\" -w \"%0\" %1 -g 0x0 -b %2 %3").arg(
                             QFileInfo(firmwareFile).absoluteFilePath(), verify ? "-v" : "", QString::number(baud), portLocation
                         );

    _firmwareProcess = QSharedPointer<QProcess>(new QProcess);
    _firmwareProcess->setEnvironment(QProcess::systemEnvironment());
    _firmwareProcess->setProcessChannelMode(QProcess::MergedChannels);
    qCDebug(PING_PROTOCOL_PING) << "3... 2... 1...";
    qCDebug(PING_PROTOCOL_PING) << cmd;
    _firmwareProcess->start(cmd);
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
                QThread::msleep(500);
                detector()->scan();
            } else {
                emit flashProgress(_fw_update_perc);
            }
        }
    }

    qCDebug(PING_PROTOCOL_PING) << output;
}

void Ping::request(int id)
{
    if(link()->type() <= LinkType::File ||
            link()->type() == LinkType::PingSimulation) {
        qCWarning(PING_PROTOCOL_PING) << "Can't write in this type of link.";
        return;
    }
    qCDebug(PING_PROTOCOL_PING) << "Requesting:" << id;

    ping_msg_ping1D_empty m;
    m.set_id(id);
    m.updateChecksum();
    writeMessage(m);

    // Add requested id
    requestedIds[static_cast<Ping1DNamespace::msg_ping1D_id>(id)].waiting++;
}

void Ping::setLastPingConfiguration()
{
    static int lastSrcId = -1;
    if(lastSrcId == _srcId) {
        return;
    }
    lastSrcId = _srcId;

    if(!link()->isWritable()) {
        qCDebug(PING_PROTOCOL_PING) << "It's not possible to set last configuration when link is writable.";
        return;
    }

    // Load previous configuration with device id
    loadLastPingConfigurationSettings();

    // Print last configuration
    QString output = QStringLiteral("\nPingConfiguration {\n");
    for(const auto& key : _pingConfiguration.keys()) {
        output += QString("\t%1: %2\n").arg(key).arg(_pingConfiguration[key].value);
    }
    output += QStringLiteral("}");
    qCDebug(PING_PROTOCOL_PING).noquote() << output;


    // Set loaded configuration in device
    static QString debugMessage =
        QStringLiteral("Device configuration does not match. Waiting for (%1), got (%2) for %3");
    static auto lastPingConfigurationTimer = new QTimer();
    connect(lastPingConfigurationTimer, &QTimer::timeout, this, [this] {
        bool stopLastPingConfigurationTimer = true;
        for(const auto& key : _pingConfiguration.keys())
        {
            auto& dataStruct = _pingConfiguration[key];
            if(dataStruct.value != dataStruct.getClassValue()) {
                qCDebug(PING_PROTOCOL_PING) <<
                                            debugMessage.arg(dataStruct.value).arg(dataStruct.getClassValue()).arg(key);
                dataStruct.setClassValue(dataStruct.value);
                stopLastPingConfigurationTimer = false;
            }
            if(key.contains("automaticMode") && dataStruct.value) {
                qCDebug(PING_PROTOCOL_PING) << "Device was running with last configuration in auto mode.";
                // If it's running in automatic mode
                // no further configuration is necessary
                break;
            }
        }
        if(stopLastPingConfigurationTimer)
        {
            qCDebug(PING_PROTOCOL_PING) << "Last configuration done, timer will stop now.";
            lastPingConfigurationTimer->stop();
            do_continuous_start(Ping1DNamespace::Profile);
        }
    });
    lastPingConfigurationTimer->start(500);
    lastPingConfigurationTimer->start();
}

void Ping::setPingFrequency(float pingFrequency)
{
    if (pingFrequency <= 0 || pingFrequency > _pingMaxFrequency) {
        qCWarning(PING_PROTOCOL_PING) << "Invalid frequency:" << pingFrequency;
        do_continuous_stop(Ping1DNamespace::Profile);
    } else {
        int periodMilliseconds = 1000.0f / pingFrequency;
        qCDebug(PING_PROTOCOL_PING) << "Setting frequency(Hz) and period(ms):" << pingFrequency << periodMilliseconds;
        set_ping_interval(periodMilliseconds);
        do_continuous_start(Ping1DNamespace::Profile);
    }
    qCDebug(PING_PROTOCOL_PING) << "Ping frequency" << pingFrequency;
}

void Ping::printStatus()
{
    qCDebug(PING_PROTOCOL_PING) << "Ping Status:";
    qCDebug(PING_PROTOCOL_PING) << "\t- srcId:" << _srcId;
    qCDebug(PING_PROTOCOL_PING) << "\t- dstID:" << _dstId;
    qCDebug(PING_PROTOCOL_PING) << "\t- device_type:" << _device_type;
    qCDebug(PING_PROTOCOL_PING) << "\t- device_model:" << _device_model;
    qCDebug(PING_PROTOCOL_PING) << "\t- firmware_version_major:" << _firmware_version_major;
    qCDebug(PING_PROTOCOL_PING) << "\t- firmware_version_minor:" << _firmware_version_minor;
    qCDebug(PING_PROTOCOL_PING) << "\t- distance:" << _distance;
    qCDebug(PING_PROTOCOL_PING) << "\t- confidence:" << _confidence;
    qCDebug(PING_PROTOCOL_PING) << "\t- pulse_duration:" << _pulse_duration;
    qCDebug(PING_PROTOCOL_PING) << "\t- ping_number:" << _ping_number;
    qCDebug(PING_PROTOCOL_PING) << "\t- start_mm:" << _scan_start;
    qCDebug(PING_PROTOCOL_PING) << "\t- length_mm:" << _scan_length;
    qCDebug(PING_PROTOCOL_PING) << "\t- gain_index:" << _gain_index;
    qCDebug(PING_PROTOCOL_PING) << "\t- mode_auto:" << _mode_auto;
    qCDebug(PING_PROTOCOL_PING) << "\t- ping_interval:" << _ping_interval;
    qCDebug(PING_PROTOCOL_PING) << "\t- points:" << QByteArray((const char*)_points.data(), _num_points).toHex(',');
}

void Ping::writeMessage(const PingMessage &msg)
{
    if(link()) {
        if(link()->isOpen() && link()->type() != LinkType::File) {
            // todo add link::write(char*, int size)
            emit link()->sendData(QByteArray(reinterpret_cast<const char*>(msg.msgData), msg.msgDataLength()));
        }
    }
}

void Ping::checkNewFirmwareInGitHubPayload(const QJsonDocument& jsonDocument)
{
    struct {
        float version = 0.0;
        QString downloadUrl;
    } versionAvailable;

    auto filesPayload = jsonDocument.array();
    for(const QJsonValue& filePayload : filesPayload) {
        qCDebug(PING_PROTOCOL_PING) << filePayload["name"].toString();

        // Get version from Ping_V(major).(patch)_115kb.hex where (major).(patch) is <version>
        static const QRegularExpression versionRegex(QStringLiteral(R"(Ping_V(?<version>\d+\.\d+)_115kb\.hex)"));
        auto filePayloadVersion = versionRegex.match(filePayload["name"].toString()).captured("version").toFloat();
        if(filePayloadVersion > versionAvailable.version) {
            versionAvailable.version = filePayloadVersion;
            versionAvailable.downloadUrl = filePayload["download_url"].toString();
        }
    }

    auto sensorVersion = QString("%1.%2").arg(_firmware_version_major).arg(_firmware_version_minor).toFloat();
    if(versionAvailable.version > sensorVersion) {
        QString newVersionText =
            QStringLiteral("Firmware update for Ping available: %1<br>").arg(versionAvailable.version) +
            QStringLiteral("<a href=\"%1\">DOWNLOAD IT HERE!</a>").arg(versionAvailable.downloadUrl);
        NotificationManager::self()->create(newVersionText, "green", StyleManager::infoIcon());
    }
}

Ping::~Ping()
{
    updatePingConfigurationSettings();
}

QDebug operator<<(QDebug d, const Ping::messageStatus& other)
{
    return d << "waiting: " << other.waiting << ", ack: " << other.ack << ", nack: " << other.nack;
}
