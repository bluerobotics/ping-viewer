#include <QNetworkDatagram>
#include <QNetworkInterface>
#include <QQmlEngine>
#include <QUdpSocket>

#include "logger.h"
#include "networkmanager.h"
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
    _broadcastSocket.bind(
        QHostAddress::AnyIPv4, randomPort, QUdpSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);
    _broadcastSocket.setSocketOption(QAbstractSocket::MulticastTtlOption, 1);

    // Bind all available interfaces that are up, running and can broadcast. Down adapters
    // (e.g. a disconnected Wi-Fi or Bluetooth interface) are skipped.
    const auto interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface& networkInterface : interfaces) {
        if (NetworkManager::isInterfaceUsable(networkInterface)
            && networkInterface.flags().testFlag(QNetworkInterface::CanBroadcast)) {
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
        if (NetworkManager::isInterfaceUsable(interface)) {
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

    // Also send to the limited broadcast address (255.255.255.255). A device on a link-local
    // AutoIP address is not on any of our subnets, so a subnet-directed broadcast may never reach
    // it; the limited broadcast improves the odds that such a device receives the discovery
    // request and replies, making detection more reliable.
    if (_broadcastSocket.writeDatagram(datagram, QHostAddress::Broadcast, Ping360AsciiProtocol::udpPort()) == -1) {
        qDebug() << "Failed to send datagram to limited broadcast address: 255.255.255.255";
    }
}

void Ping360HelperService::processBroadcastResponses()
{
    while (_broadcastSocket.hasPendingDatagrams()) {
        QNetworkDatagram datagram = _broadcastSocket.receiveDatagram();

        const Ping360DiscoveryResponse decoded = Ping360AsciiProtocol::decodeDiscoveryResponse(datagram.data());
        if (!decoded.deviceName.contains("PING360")) {
            qCWarning(PING360HELPERSERVICE) << "Invalid message:" << datagram.data();
            continue;
        }

        // A Ping360 that has not been assigned an IP address falls back to a link-local
        // (169.254.x.x) AutoIP address. It answers the discovery broadcast, but it cannot be
        // reached by unicast unless the computer is on the same subnet. Detect that case so the
        // device is flagged as needing IP configuration instead of being shown as ready to use.
        const bool reachable = NetworkManager::isAddressInSubnet(decoded.ipAddress);
        if (!reachable) {
            qCWarning(PING360HELPERSERVICE)
                << "Ping360 discovered on unreachable address:" << decoded.ipAddress
                << "- the device and the computer are on different networks. Set a static IP on the device.";
        }

        emit availableLinkFound(
            {{LinkType::Udp, {decoded.ipAddress, "12345"}, "Ping360 Port", PingDeviceType::PING360}},
            QStringLiteral("Ping360 Ethernet Protocol Detector"));
    }
}

void Ping360HelperService::setDHCPServer(const QString& ip) { setStaticIP(ip, "0.0.0.0"); }

void Ping360HelperService::setStaticIP(const QString& ip, const QString& staticIp)
{
    Q_UNUSED(ip)
    const QByteArray datagram = Ping360AsciiProtocol::staticIpAddressMessage(staticIp);
    const auto port = Ping360AsciiProtocol::udpPort();
    // Broadcast to the limited broadcast address so the message reaches a device that is currently
    // on a different subnet (e.g. a link-local AutoIP address), which a unicast to `ip` could not.
    qCDebug(PING360HELPERSERVICE) << "Broadcasting IP configuration message:" << datagram << "on port" << port;
    _broadcastSocket.writeDatagram(datagram, QHostAddress::Broadcast, port);
}

QObject* Ping360HelperService::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}
