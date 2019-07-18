#include <QDebug>
#include <QLoggingCategory>
#include <QSerialPortInfo>
#include <QTimer>

#include "seriallink.h"

Q_LOGGING_CATEGORY(PING_PROTOCOL_SERIALLINK, "ping.protocol.seriallink")

SerialLink::SerialLink(QObject* parent)
    : AbstractLink(parent)
{
    setType(LinkType::Serial);

    connect(&_port, &QIODevice::readyRead, this, [this]() {
        emit newData(_port.readAll());
    });

    connect(this, &AbstractLink::sendData, this, [this](const QByteArray& data) {
        _port.write(data);
    });

    connect(&_port, &QSerialPort::errorOccurred, this, [this](QSerialPort::SerialPortError error) {
        switch(error) {
        case QSerialPort::NoError:
            break;
        default:
            qCWarning(PING_PROTOCOL_SERIALLINK) << "Error is critical ! Port need to be closed.";
            qCWarning(PING_PROTOCOL_SERIALLINK) << "Error:" << error;
            finishConnection();
            break;
        }
    });
}

bool SerialLink::setConfiguration(const LinkConfiguration& linkConfiguration)
{
    _linkConfiguration = linkConfiguration;
    qCDebug(PING_PROTOCOL_SERIALLINK) << linkConfiguration;
    if(!linkConfiguration.isValid()) {
        qCDebug(PING_PROTOCOL_SERIALLINK) << LinkConfiguration::errorToString(linkConfiguration.error());
        return false;
    }

    setName(linkConfiguration.name());

    _port.setPortName(linkConfiguration.args()->at(0));
    _port.setBaudRate(linkConfiguration.args()->at(1).toInt());

    return true;
}

bool SerialLink::finishConnection()
{
    if(_port.isOpen()) {
        _port.close();
        qCDebug(PING_PROTOCOL_SERIALLINK) << "Port closed.";
    }
    return true;
}

QStringList SerialLink::listAvailableConnections()
{
    auto ports = QSerialPortInfo::availablePorts();
    QStringList currentPortList;
    currentPortList.reserve(ports.size());
    for(const auto& port : ports) {
        currentPortList.append(port.portName());
    }

    if(_availableConnections != currentPortList) {
        _availableConnections = currentPortList;
        emit availableConnectionsChanged();
    }

    return _availableConnections;
}

SerialLink::~SerialLink()
{
    finishConnection();
}
