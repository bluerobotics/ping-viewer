#include "ping360.h"

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
    ,_data(_sensorTimeout, 0)
{
    setControlPanel({"qrc:/Ping360ControlPanel.qml"});
    setSensorVisualizer({"qrc:/Ping360Visualizer.qml"});

    connect(this, &Sensor::connectionOpen, this, &Ping360::startPreConfigurationProcess);

    // Add timer for worst case scenario
    _timeoutProfileMessage.setInterval(_sensorTimeout);

    connect(&_timeoutProfileMessage, &QTimer::timeout, this, [this] {
        qCWarning(PING_PROTOCOL_PING360) << "Profile message timeout, new request will be done.";
        requestNextProfile();
    });
}

void Ping360::startPreConfigurationProcess()
{
    // Fetch sensor configuration to update class variables
    // Ping base class should abstract the request message to allow version compatibility between protocol versions
    common_general_request msg;
    msg.set_requested_id(Ping360Id::DEVICE_DATA);
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
    deltaStep(steps);
}

void Ping360::handleMessage(const ping_message& msg)
{
    qCDebug(PING_PROTOCOL_PING360) << "Handling Message:" << msg.message_id();

    switch (msg.message_id()) {

    case Ping360Id::DEVICE_DATA: {
        // Parse message
        const ping360_device_data deviceData(msg);
        deviceData.mode();
        _angle = deviceData.angle();
        _gain_setting = deviceData.gain_setting();
        _transmit_duration = deviceData.transmit_duration();
        _sample_period = deviceData.sample_period();
        _transmit_frequency = deviceData.transmit_frequency();

        _data.resize(deviceData.number_of_samples());
        for (int i = 0; i < deviceData.data_length(); i++) {
            _data.replace(i, deviceData.data()[i] / 255.0);
        }

        // TODO: doublecheck what we are getting and what we want
        // some parameter combinations are not valid and the sensor will automatically adjust
        emit gainSettingChanged();
        emit angleChanged();
        emit transmitDurationChanged();
        emit samplePeriodChanged();
        emit transmitFrequencyChanged();
        emit dataChanged();

        // Update total number of pings
        _ping_number++;

        // request another transmission
        requestNextProfile();

        // Restart timer
        _timeoutProfileMessage.start();

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
