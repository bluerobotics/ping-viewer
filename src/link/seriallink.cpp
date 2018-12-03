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
        case QSerialPort::DeviceNotFoundError...QSerialPort::UnknownError:
            qCWarning(PING_PROTOCOL_SERIALLINK) << "Error is critical ! Port need to be closed.";
            qCWarning(PING_PROTOCOL_SERIALLINK) << "Error:" << error;
            finishConnection();
            break;
        default:
            qCDebug(PING_PROTOCOL_SERIALLINK) << "Error appear to be not critical. Nothing will be done about it.";
            qCWarning(PING_PROTOCOL_SERIALLINK) << "Error:" << error;
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
    _port.close();
    qCDebug(PING_PROTOCOL_SERIALLINK) << "port closed";
    return true;
}

QStringList SerialLink::listAvailableConnections()
{
    static QStringList list;
    auto oldList = list;
    list.clear();
    auto ports = QSerialPortInfo::availablePorts();
    for(const auto& port : ports) {
        list.append(port.portName());
    }
    if(oldList != list) {
        emit availableConnectionsChanged();
    }
    return list;
}
