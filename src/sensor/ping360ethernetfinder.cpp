#include <QUdpSocket>

#include "ping360ethernetfinder.h"

Ping360EthernetFinder::Ping360EthernetFinder()
{
    connect(&_broadcastTimer, &QTimer::timeout, this, &Ping360EthernetFinder::doBroadcast);
    connect(&_broadcastSocket, &QUdpSocket::readyRead, this, &Ping360EthernetFinder::processBroadcastResponses);
}

void Ping360EthernetFinder::start()
{
    _broadcastTimer.start(1000);
}

void Ping360EthernetFinder::doBroadcast()
{
    // We broadcast a "Discovery" message to 255.255.255.255 and port 30303 as
    // described in the communications manual.
    QByteArray datagram = "Discovery";
    _broadcastSocket.writeDatagram(datagram, QHostAddress::Broadcast, 30303);
}

void Ping360EthernetFinder::processBroadcastResponses()
{
    while(_broadcastSocket.hasPendingDatagrams()) {
        QHostAddress sender;
        QByteArray datagram;
        datagram.resize(_broadcastSocket.pendingDatagramSize());
        _broadcastSocket.readDatagram(datagram.data(), datagram.size(), &sender);
        // Make sure we have an IPV4 address, and not something like "::ffff:192.168.1.1"
        sender = QHostAddress(sender.toIPv4Address());
        // Basic validation to check we didn't get some lost packet from wherever
        if(datagram.contains("PING360")) {
            emit availableLinkFound({LinkType::Udp, {sender.toString(), "12345"}, "Ping360 Port", PingDeviceType::PING360},
                                    "Ping360");
        }
    }
}

void Ping360EthernetFinder::stop()
{
    _broadcastTimer.stop();
    _broadcastSocket.close();
}
