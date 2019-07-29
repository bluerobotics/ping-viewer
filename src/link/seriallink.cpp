#include <QDebug>
#include <QLoggingCategory>
#include <QSerialPortInfo>
#include <QThread>
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
    emit configurationChanged();
    return true;
}

bool SerialLink::startConnection()
{
    // Check if port was already open
    if(isOpen()) {
        qCDebug(PING_PROTOCOL_SERIALLINK) << "Serial port will be restarted.";
        finishConnection();
    }

    if(!_port.open(QIODevice::ReadWrite)) {
        qCWarning(PING_PROTOCOL_SERIALLINK) << "Fail to open serial port:" << _port.error();
        return false;
    }

    /** ABR fluxogram
     * 1. Use break to force a 0 logical state for an entire frame
     * 2. Send U (0b01010101) to allow an automatic baud rate detection
     * 3. Force a write condition in the serial using the `flush` command
     */
    _port.setBreakEnabled(true);
    QThread::usleep(500);
    _port.setBreakEnabled(false);
    QThread::msleep(11);
    _port.write("UUU");
    _port.flush();
    QThread::msleep(11);

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

SerialLink::~SerialLink()
{
    finishConnection();
}
