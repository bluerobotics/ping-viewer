#include "ping360.h"

#include <algorithm>
#include <functional>
#include <limits>

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

#include "hexvalidator.h"
#include "link/seriallink.h"
#include "mavlinkmanager.h"
#include "networkmanager.h"
#include "networktool.h"
#include "notificationmanager.h"
#include "settingsmanager.h"

#include <mavlink_msg_attitude.h>

PING_LOGGING_CATEGORY(PING_PROTOCOL_PING360, "ping.protocol.ping360")

// firmware constants
const uint16_t Ping360::_firmwareMaxNumberOfPoints = 1200;
const uint16_t Ping360::_firmwareMaxTransmitDuration = 500;
const uint16_t Ping360::_firmwareMinTransmitDuration = 5;
const uint16_t Ping360::_firmwareMinSamplePeriod = 80;
// The firmware defaults at boot
const uint8_t Ping360::_firmwareDefaultGainSetting = 0;
const uint16_t Ping360::_firmwareDefaultAngle = 0;
const uint16_t Ping360::_firmwareDefaultTransmitDuration = 32;
const uint16_t Ping360::_firmwareDefaultSamplePeriod = 80;
const uint16_t Ping360::_firmwareDefaultTransmitFrequency = 740;
const uint16_t Ping360::_firmwareDefaultNumberOfSamples = 1024;

// The default transmit frequency to operate with
const uint16_t Ping360::_viewerDefaultTransmitFrequency = 750;
const uint16_t Ping360::_viewerDefaultNumberOfSamples = _firmwareMaxNumberOfPoints;
const float Ping360::_viewerDefaultRange = 2;
const uint16_t Ping360::_viewerDefaultSamplePeriod = 88;
const uint16_t Ping360::_viewerDefaultTransmitDuration = 11;

// Physical properties of the sensor
const float Ping360::_angularSpeedGradPerMs = 400.0f / 2400.0f;

Ping360::Ping360()
    : PingSensor(PingDeviceType::PING360)
{
    // QVector crashs when constructed in initialization list
    _data = QVector<double>(_maxNumberOfPoints, 0);

    setName("Ping360");
    setControlPanel({"qrc:/Ping360ControlPanel.qml"});
    setSensorVisualizer({"qrc:/Ping360Visualizer.qml"});
    setSensorStatusModel({"qrc:/Ping360StatusModel.qml"});

    connect(this, &Sensor::connectionOpen, this, &Ping360::startPreConfigurationProcess);

    // Add timer for worst case scenario
    _timeoutProfileMessage.setInterval(_sensorTimeout);
    _baudrateConfigurationTimer.setInterval(100);

    connect(&_timeoutProfileMessage, &QTimer::timeout, this, [this] {
        qCWarning(PING_PROTOCOL_PING360) << "Profile message timeout, new request will be done.";
        requestNextProfile();

        static int timeoutedTime = 0;
        timeoutedTime += _timeoutProfileMessage.interval();
        if (timeoutedTime > _sensorRestartTimeoutMs) {
            timeoutedTime = 0;
            _timeoutProfileMessage.setInterval(_sensorTimeout);
        }
    });

    connect(&_baudrateConfigurationTimer, &QTimer::timeout, this, [this] {
        qCWarning(PING_PROTOCOL_PING360) << "Device Info timeout";
        checkBaudrateProcess();
    });

    // Start timer to calculate frequency for each message type
    _messageFrequencyTimer.setInterval(1000);
    _messageFrequencyTimer.start();

    connect(&_messageFrequencyTimer, &QTimer::timeout, this, [this] {
        for (auto& message : messageFrequencies) {
            message.updateFrequencyFromMilliseconds(_messageFrequencyTimer.interval());
        }
        // Since we don't have a huge number of messages and this variable is pretty simple,
        // we can use a single signal to update someone about the frequency update
        emit messageFrequencyChanged();
    });

    connect(MavlinkManager::self(), &MavlinkManager::mavlinkMessage, this, [](auto message) {
        mavlink_attitude_t attitude;
        mavlink_msg_attitude_decode(&message, &attitude);
        qDebug() << attitude.roll << attitude.pitch << attitude.yaw;
    });
}

void Ping360::startPreConfigurationProcess()
{
    // Force the default settings
    resetSettings();

    // Stop all configuration/message timers if link is not writable
    if (!link()->isWritable()) {
        qCDebug(PING_PROTOCOL_PING360)
            << "Not possible to start the preconfiguration process with a non-writable channel.";
        stopConfiguration();
        return;
    }

    // Baud rate configuration is only done in serial channels
    if (link()->type() != LinkType::Serial) {
        _configuring = false;
        if (_baudrateConfigurationTimer.isActive()) {
            _baudrateConfigurationTimer.stop();
        }
    }

    // Fetch sensor configuration to update class variables
    // TODO: Ping base class should abstract the request message to allow version compatibility between protocol
    // versions
    common_general_request msg;
    msg.set_requested_id(CommonId::DEVICE_INFORMATION);
    msg.updateChecksum();
    writeMessage(msg);
}

void Ping360::checkBaudrateProcess()
{
    // We use the pre configuration message to check for valid baud rates
    startPreConfigurationProcess();

    static int count = _ABRTotalNumberOfMessages;

    if (_resetBaudRateDetection) {
        count = _ABRTotalNumberOfMessages;
    }

    // We are starting to check the new baud rate
    if (count == _ABRTotalNumberOfMessages) {
        detectBaudrates();
    }

    if (count--) {
        _baudrateConfigurationTimer.start();
    } else {
        count = _ABRTotalNumberOfMessages;
    }
}

void Ping360::loadLastSensorConfigurationSettings()
{
    // TODO
}

void Ping360::updateSensorConfigurationSettings()
{
    // TODO
}

void Ping360::connectLink(LinkType connType, const QStringList& connString)
{
    Sensor::connectLink(LinkConfiguration {connType, connString});
}

void Ping360::requestNextProfile()
{
    // Calculate the next delta step
    int steps = _angular_speed;
    if (_reverse_direction) {
        steps *= -1;
    }

    // Check if steps is in sector
    auto isInside = [this](int iSteps) -> bool {
        int relativeAngle = (iSteps + angle() + _angularResolutionGrad) % _angularResolutionGrad;
        if (relativeAngle >= _angularResolutionGrad / 2) {
            relativeAngle -= _angularResolutionGrad;
        }
        return std::clamp(relativeAngle, -_sectorSize / 2, _sectorSize / 2) == relativeAngle;
    };

    // Move the other direction to be in sector
    if (!isInside(steps)) {
        _reverse_direction = !_reverse_direction;
        steps *= -1;
    }

    // If we are not inside yet, we are not in section, go to zero
    if (!isInside(steps)) {
        _reverse_direction = !_reverse_direction;
        steps = -angle();
    }

    deltaStep(steps);
}

void Ping360::handleMessage(const ping_message& msg)
{
    qCDebug(PING_PROTOCOL_PING360) << QStringLiteral("Handling Message: %1 [%2]")
                                          .arg(PingHelper::nameFromMessageId(
                                              static_cast<PingEnumNamespace::PingMessageId>(msg.message_id())))
                                          .arg(msg.message_id());

    switch (msg.message_id()) {

    case CommonId::DEVICE_INFORMATION: {
        // Stop all configuration/message timers if link is not writable
        if (!link()->isWritable()) {
            stopConfiguration();
            return;
        }

        // Baud rate configuration is only done in serial channels
        if (_configuring && link()->type() == LinkType::Serial) {
            _baudrateConfigurationTimer.start();
            checkBaudrateProcess();
        } else {
            _baudrateConfigurationTimer.stop();
            _timeoutProfileMessage.start();
            requestNextProfile();
        }
        return;
    }

    case Ping360Id::DEVICE_DATA: {
        // Parse message
        const ping360_device_data deviceData = *static_cast<const ping360_device_data*>(&msg);

        // Get angle to request next message
        _angle = deviceData.angle();

        // Request next message ASAP
        // request another transmission
        requestNextProfile();

        // Restart timer, if the channel allows it
        if (link()->isWritable()) {
            // Use 200ms for network delay
            const int profileRunningTimeout = _angular_speed / _angularSpeedGradPerMs + 200;
            _timeoutProfileMessage.start(profileRunningTimeout);
        }

        _data.resize(deviceData.data_length());
#pragma omp for
        for (int i = 0; i < deviceData.data_length(); i++) {
            _data.replace(i, deviceData.data()[i] / 255.0);
        }

        // TODO: doublecheck what we are getting and what we want
        // some parameter combinations are not valid and the sensor will automatically adjust
        // in order to detect this, we will have to track our last commanded values separately
        // from our presently commanded values
        emit angleChanged();

        // Only emit data changed when inside sector range
        if (_data.size()) {
            // Update total number of pings
            _ping_number++;

            if (_sectorSize == 400 || (angle() >= _angularResolutionGrad - _sectorSize / 2)
                || (angle() <= _sectorSize / 2)) {
                emit dataChanged();
            }
        }

        // This properties are changed internally only when the link is not writable
        // Such information is normally sync between our application and the sensor
        // So with normal links such attribution is not necessary
        _sensorSettings.checkValidation({deviceData.transmit_duration(), deviceData.gain_setting(),
            deviceData.data_length(), deviceData.sample_period(), deviceData.transmit_frequency()});
        // Everything should be valid, otherwise the sensor is not in sync
        if (!link()->isWritable() && _sensorSettings.valid) {
            set_gain_setting(deviceData.gain_setting());
            set_transmit_duration(deviceData.transmit_duration());
            set_sample_period(deviceData.sample_period());
            set_transmit_frequency(deviceData.transmit_frequency());
            set_number_of_points(deviceData.data_length());
        }

        break;
    }

    case CommonId::NACK: {
        const common_nack nack(msg);
        if (nack.nacked_id() == Ping360Id::TRANSDUCER) {
            qCWarning(PING_PROTOCOL_PING360) << "transducer control was NACKED, reverting to default settings";

            set_gain_setting(_firmwareDefaultGainSetting);
            set_transmit_duration(_viewerDefaultTransmitDuration);
            set_sample_period(_viewerDefaultSamplePeriod);
            set_transmit_frequency(_viewerDefaultTransmitFrequency);
            set_number_of_points(_viewerDefaultNumberOfSamples);

            // request another transmission
            requestNextProfile();

            // restart timer
            _timeoutProfileMessage.start();

            emit gainSettingChanged();
            emit samplePeriodChanged();
            emit transmitFrequencyChanged();
            emit numberOfPointsChanged();
            emit rangeChanged();
        }
        break;
    }

    default:
        qWarning(PING_PROTOCOL_PING360) << "UNHANDLED MESSAGE ID:" << msg.message_id();
        break;
    }

    // Update frequency for each
    messageFrequencies[msg.message_id()].updateNumberOfMessages();

    emit parsedMsgsUpdate();
}

void Ping360::firmwareUpdate(QString fileUrl, bool sendPingGotoBootloader, int baud, bool verify)
{
    Q_UNUSED(fileUrl)
    Q_UNUSED(sendPingGotoBootloader)
    Q_UNUSED(baud)
    Q_UNUSED(verify)
    // TODO
}

void Ping360::flash(const QString& fileUrl, bool sendPingGotoBootloader, int baud, bool verify)
{
    Q_UNUSED(fileUrl)
    Q_UNUSED(sendPingGotoBootloader)
    Q_UNUSED(baud)
    Q_UNUSED(verify)
    // TODO
}

void Ping360::setLastSensorConfiguration()
{
    // TODO
}

void Ping360::printSensorInformation() const
{
    qCDebug(PING_PROTOCOL_PING360) << "Ping360 Status:";
    // TODO
}

void Ping360::checkNewFirmwareInGitHubPayload(const QJsonDocument& jsonDocument)
{
    Q_UNUSED(jsonDocument)
    // TODO
}

void Ping360::resetSensorLocalVariables()
{
    // TODO
}

const QList<int>& Ping360::validBaudRates()
{
    static QList<int> validBaudRates;
    if (validBaudRates.isEmpty()) {
        for (const auto& variant : _validBaudRates) {
            validBaudRates.append(variant.toInt());
        }
    }
    return validBaudRates;
}

const QVariantList& Ping360::validBaudRatesAsVariantList() const { return _validBaudRates; }

void Ping360::setBaudRate(int baudRate)
{
    // It's only possible to change baudrates in serial connections
    if (link()->type() != LinkType::Serial) {
        return;
    }

    // Since ping360 uses automatic baudrate detection
    // it's necessary to start the connection to force baud rate changes
    SerialLink* serialLink = dynamic_cast<SerialLink*>(link());
    if (!serialLink) {
        qCWarning(PING_PROTOCOL_PING360) << "Link is serial type, but cast was not possible!";
        return;
    }

    qCDebug(PING_PROTOCOL_PING360) << "Moving to baud rate:" << baudRate;
    serialLink->setBaudRate(baudRate);
    emit linkUpdate();
}

void Ping360::setBaudRateAndRequestProfile(int baudRate)
{
    setBaudRate(baudRate);
    QThread::msleep(100);
    requestNextProfile();
}

void Ping360::detectBaudrates()
{
    // Check all valid baudrates
    static int index = 0;

    // baudrate to error
    static QMap<int, int> baudRateToError;

    // last parser error count
    static int lastParserErrorCount = 0;
    static int lastParserMsgsCount = 0;

    if (_resetBaudRateDetection) {
        _resetBaudRateDetection = false;
        index = 0;
        baudRateToError.clear();
        lastParserErrorCount = 0;
        lastParserMsgsCount = 0;
    };

    // Check error margin
    int lastCounter = -1;

    if (!_configuring) {
        return;
    }

    // We have already something to calculate
    if (index != 0) {
        // Get previous baud rate and error margin
        lastCounter =
            // Number of messages that failed to parse
            parserErrors() - lastParserErrorCount +
            // Number of messages that was lost
            _ABRTotalNumberOfMessages - (parsedMsgs() - lastParserMsgsCount);

        baudRateToError[validBaudRates()[index - 1]] = lastCounter;
    }
    lastParserErrorCount = parserErrors();
    lastParserMsgsCount = parsedMsgs();

    // We are in the end of the list or someone is the winner!
    if (index == validBaudRates().size() - 1 || lastCounter == 0) {
        _configuring = false;

        // The actual baud rate is the winner, don't need to set a new one
        if (lastCounter == 0) {
            qCDebug(PING_PROTOCOL_PING360) << "Baud rate procedure done.";
            return;
        }

        // We are not in the end of the list, so there is a valid baud rate that is faster
        // than the lowest speed
        if (index != validBaudRates().size()) {
            index = 0;
            while (baudRateToError[validBaudRates()[index]] != 0) {
                index++;
            }
        } else {
            // We are in the end of the list, we are going to use the lowest baud rate
            index = validBaudRates().size() - 1;
        }

        setBaudRate(validBaudRates()[index]);
        qCDebug(PING_PROTOCOL_PING360) << "Baud rate procedure done.";
        return;
    }

    // Check next baud rate
    setBaudRate(validBaudRates()[index]);
    index++;
}

void Ping360::stopConfiguration()
{
    _configuring = false;
    if (_timeoutProfileMessage.isActive()) {
        _timeoutProfileMessage.stop();
    }
    if (_baudrateConfigurationTimer.isActive()) {
        _baudrateConfigurationTimer.stop();
    }
}

uint16_t Ping360::calculateSamplePeriod(float distance)
{
    float calculatedSamplePeriod
        = 2.0f * distance / (_sensorSettings.num_points * _speed_of_sound * _samplePeriodTickDuration);
    if (qFuzzyIsNull(calculatedSamplePeriod) || calculatedSamplePeriod < 0
        || calculatedSamplePeriod > std::numeric_limits<uint16_t>::max()) {
        qCWarning(PING_PROTOCOL_PING360) << "Invalid calculation of sample period. Going to use viewer default values.";
        qCDebug(PING_PROTOCOL_PING360) << "calculatedSamplePeriod: " << calculatedSamplePeriod
                                       << "distance:" << distance << "_num_points" << _sensorSettings.num_points
                                       << "_speed_of_sound" << _speed_of_sound << "_samplePeriodTickDuration"
                                       << _samplePeriodTickDuration;

        return _viewerDefaultSamplePeriod;
    }

    return static_cast<uint16_t>(calculatedSamplePeriod);
}

void Ping360::resetSettings()
{
    qCDebug(PING_PROTOCOL_PING360) << "Settings will be reseted.";

    set_gain_setting(_firmwareDefaultGainSetting);
    set_transmit_duration(_viewerDefaultTransmitDuration);
    set_sample_period(_viewerDefaultSamplePeriod);
    set_transmit_frequency(_viewerDefaultTransmitFrequency);
    set_number_of_points(_viewerDefaultNumberOfSamples);
    set_range(_viewerDefaultRange);
    // Signals will be update in the next profile, it's possible that old profiles contain older configurations
    // Turn sensor settings invalid and let the interface handle the sync
    _sensorSettings.valid = false;
}

Ping360::~Ping360() { updateSensorConfigurationSettings(); }
