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

const int Ping360::_pingMaxFrequency = 50;

Ping360::Ping360()
    :PingSensor()
    ,_data(_num_points, 0)
{
}

void Ping360::startPreConfigurationProcess()
{
    //TODO
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

void Ping360::handleMessage(const ping_message& msg)
{
    qCDebug(PING_PROTOCOL_PING360) << "Handling Message:" << msg.message_id();

    switch (msg.message_id()) {

    case Ping360Namespace::DeviceData: {
        const ping360_device_data deviceData(msg);
        deviceData.mode();
        _gain_setting = deviceData.gain_setting();
        deviceData.angle();
        _transmit_duration = deviceData.transmit_duration();
        deviceData.transmit_duration();
        deviceData.transmit_frequency();
        deviceData.number_of_samples();
        deviceData.data_length();
        deviceData.data();

        for (int i = 0; i < deviceData.data_length(); i++) {
            _data.replace(i, deviceData.data()[i] / 255.0);
        }
        emit dataChanged();
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

void Ping360::setPingFrequency(float pingFrequency)
{
    Q_UNUSED(pingFrequency)
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

QQmlComponent* Ping360::sensorVisualizer(QObject *parent)
{
    QQmlEngine *engine = qmlEngine(parent);
    if(!engine) {
        qCDebug(PING_PROTOCOL_PING360) << "No qml engine to load visualization.";
        return nullptr;
    }
    QQmlComponent *component = new QQmlComponent(engine, QUrl("qrc:/Ping360Visualizer.qml"));
    return component;
}

Ping360::~Ping360()
{
    updateSensorConfigurationSettings();
}
