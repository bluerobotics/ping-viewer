#include "ping360.h"

#include <algorithm>
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

#include "hexvalidator.h"
#include "link/seriallink.h"
#include "networkmanager.h"
#include "networktool.h"
#include "notificationmanager.h"
#include "settingsmanager.h"

Q_LOGGING_CATEGORY(PING_PROTOCOL_PING360, "ping.protocol.ping360")

Ping360::Ping360()
    :PingSensor()
{
    // QVector crashs when constructed in initialization list
    _data = QVector<double>(_maxNumberOfPoints, 0);

    setControlPanel({"qrc:/Ping360ControlPanel.qml"});
    setSensorVisualizer({"qrc:/Ping360Visualizer.qml"});

    connect(this, &Sensor::connectionOpen, this, &Ping360::startPreConfigurationProcess);

    // Add timer for worst case scenario
    _timeoutProfileMessage.setInterval(_sensorTimeout);

    connect(&_timeoutProfileMessage, &QTimer::timeout, this, [this] {
        qCWarning(PING_PROTOCOL_PING360) << "Profile message timeout, new request will be done.";
        // Since ping360 uses automatic baudrate detection
        // it's necessary to start the connection to force baud rate changes
        link()->startConnection();
        requestNextProfile();
    });

    // Start timer to calculate frequency for each message type
    _messageElapsedTimer.start();
}

void Ping360::startPreConfigurationProcess()
{
    // Fetch sensor configuration to update class variables
    // TODO: Ping base class should abstract the request message to allow version compatibility between protocol versions
    common_general_request msg;
    msg.set_requested_id(CommonId::DEVICE_INFORMATION);
    msg.updateChecksum();
    writeMessage(msg);
    _timeoutProfileMessage.start();
}

void Ping360::loadLastSensorConfigurationSettings()
{
    //TODO
}

void Ping360::updateSensorConfigurationSettings()
{
    //TODO
}

void Ping360::connectLink(LinkType connType, const QStringList& connString)
{
    Sensor::connectLink(LinkConfiguration{connType, connString});
    startPreConfigurationProcess();
}

void Ping360::requestNextProfile()
{
    // Calculate the next depta step
    int steps = _angular_speed;
    if(_reverse_direction) {
        steps *= -1;
    }

    // Check if steps is in sector
    auto isInside = [this](int steps) -> bool {
        int relativeAngle = (steps + angle() + _angularResolutionGrad)%_angularResolutionGrad;
        if(relativeAngle >= _angularResolutionGrad/2)
        {
            relativeAngle -= _angularResolutionGrad;
        }
        return std::clamp(relativeAngle, -_sectorSize/2, _sectorSize/2) == relativeAngle;
    };

    // Move the other direction to be in sector
    if(!isInside(steps)) {
        _reverse_direction = !_reverse_direction;
        steps *= -1;
    }

    // If we are not inside yet, we are not in section, go to zero
    if(!isInside(steps)) {
        _reverse_direction = !_reverse_direction;
        steps = -angle();
    }

    deltaStep(steps);
}

void Ping360::handleMessage(const ping_message& msg)
{
    qCDebug(PING_PROTOCOL_PING360) << "Handling Message:" << msg.message_id();

    // Update frequency for each
    messageFrequencies[msg.message_id()].setElapsed(_messageElapsedTimer.elapsed());
    // Since we don't have a huge number of messages and this variable is pretty simple,
    // we can use a single signal to update someone about the frequency update
    emit messageFrequencyChanged();

    switch (msg.message_id()) {

    case Ping360Id::DEVICE_DATA: {
        // Parse message
        const ping360_device_data deviceData(msg);

        _angle = deviceData.angle();

        _data.resize(deviceData.data_length());
        for (int i = 0; i < deviceData.data_length(); i++) {
            _data.replace(i, deviceData.data()[i] / 255.0);
        }

        // TODO: doublecheck what we are getting and what we want
        // some parameter combinations are not valid and the sensor will automatically adjust
        // in order to detect this, we will have to track our last commanded values separately
        // from our presently commanded values
        emit angleChanged();

        // Only emit data changed when inside sector range
        if(_data.size()) {
            // Update total number of pings
            _ping_number++;

            if (_sectorSize == 400
                    || (angle() >= _angularResolutionGrad - _sectorSize/2) || (angle() <= _sectorSize/2)) {
                emit dataChanged();
            }
        }

        // request another transmission
        requestNextProfile();

        // Restart timer
        _timeoutProfileMessage.start();

        break;
    }

    case CommonId::NACK: {
        const common_nack nack(msg);
        if (nack.nacked_id() == Ping360Id::TRANSDUCER) {
            qCWarning(PING_PROTOCOL_PING360) << "transducer control was NACKED, reverting to default settings";

            _gain_setting = _firmwareDefaultGainSetting;
            _transmit_duration = _firmwareDefaultTransmitDuration;
            _sample_period = _firmwareDefaultSamplePeriod;
            _transmit_frequency = _viewerDefaultTransmitFrequency;
            _num_points = _viewerDefaultNumberOfSamples;

            // request another transmission
            requestNextProfile();

            // restart timer
            _timeoutProfileMessage.start();

            emit gainSettingChanged();
            emit transmitDurationChanged();
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
    emit parsedMsgsUpdate();
}

void Ping360::firmwareUpdate(QString fileUrl, bool sendPingGotoBootloader, int baud, bool verify)
{
    Q_UNUSED(fileUrl)
    Q_UNUSED(sendPingGotoBootloader)
    Q_UNUSED(baud)
    Q_UNUSED(verify)
    //TODO
}

void Ping360::flash(const QString& fileUrl, bool sendPingGotoBootloader, int baud, bool verify)
{
    Q_UNUSED(fileUrl)
    Q_UNUSED(sendPingGotoBootloader)
    Q_UNUSED(baud)
    Q_UNUSED(verify)
    //TODO
}

void Ping360::setLastSensorConfiguration()
{
    //TODO
}

void Ping360::printSensorInformation() const
{
    qCDebug(PING_PROTOCOL_PING360) << "Ping360 Status:";
    //TODO
}

void Ping360::checkNewFirmwareInGitHubPayload(const QJsonDocument& jsonDocument)
{
    Q_UNUSED(jsonDocument)
    //TODO
}

void Ping360::resetSensorLocalVariables()
{
    //TODO
}

Ping360::~Ping360()
{
    updateSensorConfigurationSettings();
}
