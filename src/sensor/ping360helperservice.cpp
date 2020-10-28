#include <QQmlEngine>
#include <QUdpSocket>

#include "logger.h"
#include "ping360asciiprotocol.h"
#include "ping360helperservice.h"

PING_LOGGING_CATEGORY(PING360HELPERSERVICE, "ping.ping360helperservice");

Ping360HelperService* Ping360HelperService::self()
{
    static Ping360HelperService self;
    return &self;
}

Ping360HelperService::Ping360HelperService()
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    connect(&_broadcastTimer, &QTimer::timeout, this, &Ping360HelperService::doBroadcast);
    connect(&_broadcastSocket, &QUdpSocket::readyRead, this, &Ping360HelperService::processBroadcastResponses);
    connect(
        &_broadcastSocket, &QAbstractSocket::errorOccurred, this, [this](QAbstractSocket::SocketError socketError) {
            qCWarning(PING360HELPERSERVICE)
                << QStringLiteral("Socket error[%1]: %2").arg(socketError).arg(_broadcastSocket.errorString());
        });
}

void Ping360HelperService::startBroadcastService() { _broadcastTimer.start(1000); }

void Ping360HelperService::stopBroadcastService()
{
    _broadcastTimer.stop();
    _broadcastSocket.close();
}

void Ping360HelperService::doBroadcast()
{
    const QByteArray datagram = Ping360AsciiProtocol::discoveryMessage();

    // Send discovery message as broadcast and for companion ip subnet
    for (const QHostAddress& address : {QHostAddress {QHostAddress::Broadcast}, QHostAddress {"192.168.2.255"}}) {
        _broadcastSocket.writeDatagram(datagram, address, Ping360AsciiProtocol::udpPort());
    }
}

void Ping360HelperService::processBroadcastResponses()
{
    while (_broadcastSocket.hasPendingDatagrams()) {
        QHostAddress sender;
        QByteArray datagram;
        datagram.resize(_broadcastSocket.pendingDatagramSize());
        _broadcastSocket.readDatagram(datagram.data(), datagram.size(), &sender);
        // Make sure we have an IPV4 address, and not something like "::ffff:192.168.1.1"
        sender = QHostAddress(sender.toIPv4Address());

        const Ping360DiscoveryResponse decoded = Ping360AsciiProtocol::decodeDiscoveryResponse(datagram);
        if (decoded.deviceName.contains("PING360")) {
            emit availableLinkFound(
                {{LinkType::Udp, {sender.toString(), "12345"}, "Ping360 Port", PingDeviceType::PING360}},
                QStringLiteral("Ping360 Ethernet Protocol Detector"));
        } else {
            qCWarning(PING360HELPERSERVICE) << "Invalid message:" << datagram;
        }
    }
}

void Ping360HelperService::setDHCPServer(const QString& ip) { setStaticIP(ip, "0.0.0.0"); }

void Ping360HelperService::setStaticIP(const QString& ip, const QString& staticIp)
{
    const QByteArray datagram = Ping360AsciiProtocol::staticIpAddressMessage(staticIp);
    qCDebug(PING360HELPERSERVICE) << "Sending IP configuration message:" << datagram;
    _broadcastSocket.writeDatagram(datagram, QHostAddress {ip}, Ping360AsciiProtocol::udpPort());
}

QObject* Ping360HelperService::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}
