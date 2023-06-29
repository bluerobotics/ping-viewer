#include <QNetworkDatagram>
#include <QNetworkInterface>
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

    const int randomPort = 0; // Force OS to give a random port
    _broadcastSocket.bind(QHostAddress::AnyIPv4, randomPort, QAbstractSocket::ReuseAddressHint);

    // Bind all available interfaces
    const auto interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& networkInterface : interfaces) {
        if (networkInterface.flags() & QNetworkInterface::CanBroadcast) {
            _broadcastSocket.joinMulticastGroup(QHostAddress(QHostAddress::Broadcast), networkInterface);
        }
    }

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

    // Retrieve the IP addresses and their associated broadcast addresses
    QList<QPair<QHostAddress, QHostAddress>> ipBroadcastAddresses;
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& interface : interfaces) {
        if (interface.flags().testFlag(QNetworkInterface::IsUp)
            && !interface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
            QList<QNetworkAddressEntry> entries = interface.addressEntries();
            for (const QNetworkAddressEntry& entry : entries) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    ipBroadcastAddresses.append(qMakePair(entry.ip(), entry.broadcast()));
                }
            }
        }
    }

    // Send discovery message as broadcast for each interface's associated broadcast address
    for (const QPair<QHostAddress, QHostAddress>& addressPair : ipBroadcastAddresses) {
        const QHostAddress& ipAddress = addressPair.first;
        const QHostAddress& broadcastAddress = addressPair.second;

        if (_broadcastSocket.writeDatagram(datagram, broadcastAddress, Ping360AsciiProtocol::udpPort()) == -1) {
            qDebug() << "Failed to send datagram to broadcast address:" << broadcastAddress.toString()
                     << "for IP address:" << ipAddress.toString();
        } else {
            qDebug() << "Datagram sent successfully to broadcast address:" << broadcastAddress.toString()
                     << "for IP address:" << ipAddress.toString();
        }
    }
}

void Ping360HelperService::processBroadcastResponses()
{
    while (_broadcastSocket.hasPendingDatagrams()) {
        QHostAddress sender, destination;

        QNetworkDatagram datagram = _broadcastSocket.receiveDatagram();

        // Make sure we have an IPV4 address, and not something like "::ffff:192.168.1.1"
        sender = QHostAddress(datagram.senderAddress().toIPv4Address());

        // if the sender ip address starts with 169.254.x.x, then the ping360 has not
        // been assigned an ip address and we will not be able to reach it on this address
        // we need to broadcast on the destination subnet, where we will be able to communicate
        if (sender.isLinkLocal()) {
            sender = QHostAddress(datagram.destinationAddress().toIPv4Address() | 255);
        }

        const Ping360DiscoveryResponse decoded = Ping360AsciiProtocol::decodeDiscoveryResponse(datagram.data());
        if (decoded.deviceName.contains("PING360")) {
            emit availableLinkFound(
                {{LinkType::Udp, {decoded.ipAddress, "12345"}, "Ping360 Port", PingDeviceType::PING360}},
                QStringLiteral("Ping360 Ethernet Protocol Detector"));
        } else {
            qCWarning(PING360HELPERSERVICE) << "Invalid message:" << datagram.data();
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
