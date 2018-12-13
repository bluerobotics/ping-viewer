#include <QtConcurrent>
#include <QDebug>
#include <QFuture>
#include <QLoggingCategory>
#include <QNetworkDatagram>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QUdpSocket>

#include "pingmessage/pingmessage.h"
#include "pingmessage/pingmessage_ping1D.h"
#include "protocoldetector.h"

Q_LOGGING_CATEGORY(PING_PROTOCOL_PROTOCOLDETECTOR, "ping.protocol.protocoldetector")

const QStringList ProtocolDetector::_invalidSerialPortNames(
{
#ifdef Q_OS_OSX
    "cu.", "SPPDev", "iPhone", "Bluetooth",
#endif
});

ProtocolDetector::ProtocolDetector()
{
    _linkConfigs.append({
        {LinkType::Udp, {"192.168.2.2", "9090"}, "BlueRov2 standard connection"},
        {LinkType::Udp, {"127.0.0.1", "1234"}, "Development port"}
    });
};

void ProtocolDetector::scan()
{
    _active = true;
    LinkConfiguration linkConf;
    // Scan until we find a ping, then stop
    while (_active) {
        auto linksConf = updateLinkConfigurations(_linkConfigs);
        for(LinkConfiguration& tryLinkConf : linksConf) {
            linkConf = tryLinkConf;
            if(checkLink(linkConf)) {
                break;
            }
        }
        QThread::msleep(500);
    }
    qCDebug(PING_PROTOCOL_PROTOCOLDETECTOR) << "Scan finished.";
}

bool ProtocolDetector::checkLink(LinkConfiguration& linkConf)
{
    _detected = false;
    if(linkConf.type() == LinkType::Udp) {
        checkUdp(linkConf);
    } else if(linkConf.type() == LinkType::Serial) {
        checkSerial(linkConf);
    } else {
        qDebug(PING_PROTOCOL_PROTOCOLDETECTOR) << "Couldn't handle configuration:" << linkConf;
    }

    if(_detected) {
        qCDebug(PING_PROTOCOL_PROTOCOLDETECTOR) << "Ping detected on:" << linkConf;
        if(!_availableLinks.contains(linkConf)) {
            _availableLinks.append(linkConf);
        }
        emit connectionDetected(linkConf);
        _active = false;
    }
    return _detected;
}

QVector<LinkConfiguration> ProtocolDetector::updateLinkConfigurations(QVector<LinkConfiguration>& linkConfig) const
{
    QVector<LinkConfiguration> tempConfigs;
    auto portsInfo = QSerialPortInfo::availablePorts();
    for(const auto& portInfo : portsInfo) {
        // Do not run with invalid ports
        if(!isValidPort(portInfo)) {
            continue;
        }

        // Add valid port
        tempConfigs.append({LinkType::Serial, {portInfo.portName(), "115200"}, QString("Detector serial link")});
    }
    return linkConfig + tempConfigs;
}

bool ProtocolDetector::checkSerial(LinkConfiguration& linkConf)
{
    // To find a ping, we this message on a link, then wait for a reply
    ping_msg_ping1D_empty req;
    req.set_id(Ping1DNamespace::Firmware_version);
    req.updateChecksum();

    QSerialPortInfo portInfo(linkConf.serialPort());
    int baudrate = linkConf.serialBaudrate();

    // Check if port can be opened
    if(!canOpenPort(portInfo, 500)) {
        qCDebug(PING_PROTOCOL_PROTOCOLDETECTOR) << "Couldn't open port" << portInfo.portName();
        return false;
    }

    QSerialPort port(portInfo);

    qCDebug(PING_PROTOCOL_PROTOCOLDETECTOR) << "Probing Serial" << port.portName() << baudrate;

    if(!port.open(QIODevice::ReadWrite)) {
        qCDebug(PING_PROTOCOL_PROTOCOLDETECTOR) << "Fail to open";
        return false;
    }
    qCDebug(PING_PROTOCOL_PROTOCOLDETECTOR) << "Port is open";
    port.setBaudRate(baudrate);

    // Probe
    port.write(reinterpret_cast<const char*>(req.msgData), (uint16_t)req.msgDataLength());
    port.waitForBytesWritten();

    int attempts = 0;

    while (!_detected && attempts < 10) { // Try to get a valid response, timeout after 10 * 50 ms
        port.waitForReadyRead(50);
        auto buf = port.readAll();
        for (const auto& byte : buf) {
            _detected = _parser.parseByte(byte) == PingParser::NEW_MESSAGE;
            if (_detected) {
                break;
            }
        }
        attempts++;
    }

    port.close();

    return _detected;
}

bool ProtocolDetector::checkUdp(LinkConfiguration& linkConf)
{
    // To find a ping, we this message on a link, then wait for a reply
    ping_msg_ping1D_empty req;
    req.set_id(Ping1DNamespace::Firmware_version);
    req.updateChecksum();

    QUdpSocket socket;

    // To test locally, change the host to 127.0.0.1 and use something like:
    // nc -kul 127.0.0.1 8888 > /dev/ttyUSB0 < /dev/ttyUSB0
    // or
    // socat UDP-LISTEN:1234,fork,reuseaddr,ignoreeof FILE:/dev/ttyUSB1,b115200,raw,ignoreeof

    qCDebug(PING_PROTOCOL_PROTOCOLDETECTOR) << "Probing UDP:" << linkConf;

    socket.writeDatagram(
        reinterpret_cast<const char*>(req.msgData), req.msgDataLength(), QHostAddress(linkConf.udpHost()), linkConf.udpPort()
    );

    int attempts = 0;

    // Try to get a valid response, timeout after 10 * 50 ms
    while (!_detected && attempts++ < 10) {
        socket.waitForReadyRead(50);
        QNetworkDatagram datagram = socket.receiveDatagram();
        auto buf = datagram.data();
        for (auto byte : buf) {
            _detected = _parser.parseByte(byte) == PingParser::NEW_MESSAGE;
            if (_detected) {
                break;
            }
        }
        attempts++;
    }

    socket.close();
    return _detected;
}

bool ProtocolDetector::canOpenPort(QSerialPortInfo& port, int msTimeout)
{
    // Call function asynchronously:
    auto checkPort = [](const QSerialPortInfo& portInfo) {
        QSerialPort port(portInfo);
        bool ok = port.open(QIODevice::ReadWrite);
        if(!ok) {
            qCWarning(PING_PROTOCOL_PROTOCOLDETECTOR) << "Fail to open serial port:" << port.error();
        }
        // Close will check if is open
        port.close();
        return ok;
    };

    QFuture<bool> future = QtConcurrent::run(checkPort, port);
    // Wait for msTimeout
    float waitForTenthOfTimeout = 0;
    while(waitForTenthOfTimeout < 10 && !future.isFinished()) {
        QThread::msleep(msTimeout/10.0f);
        qCDebug(PING_PROTOCOL_PROTOCOLDETECTOR) << "Waiting port to open.. " << waitForTenthOfTimeout << port.portName();
        waitForTenthOfTimeout += 1;
    }

    bool ok = false;
    if(future.isFinished()) {
        ok = future.result();
    }
    return ok;
}

bool ProtocolDetector::isValidPort(const QSerialPortInfo& serialPortInfo) const
{
    for(const auto& name : _invalidSerialPortNames) {
        if(serialPortInfo.portName().contains(name, Qt::CaseInsensitive)) {
            return false;
        }
    }
    return true;
}
