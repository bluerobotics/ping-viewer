#include <QDebug>
#include <QLoggingCategory>
#include <QNetworkDatagram>

#include "udplink.h"

Q_LOGGING_CATEGORY(PING_PROTOCOL_UDPLINK, "ping.protocol.udplink")

UDPLink::UDPLink(QObject* parent)
    : AbstractLink(parent)
    , _udpSocket(new QUdpSocket(parent))
{
    setType(LinkType::Udp);

    connect(_udpSocket, &QIODevice::readyRead, this, [this]() {
        emit newData(_udpSocket->receiveDatagram().data());
    });

    connect(this, &AbstractLink::sendData, this, [this](const QByteArray& data) {
        _udpSocket->writeDatagram(data, _hostAddress, _port);
    });
}

bool UDPLink::setConfiguration(const LinkConfiguration& linkConfiguration)
{
    _linkConfiguration = linkConfiguration;
    qCDebug(PING_PROTOCOL_UDPLINK) << linkConfiguration;
    if(!linkConfiguration.isValid()) {
        qCDebug(PING_PROTOCOL_UDPLINK) << LinkConfiguration::errorToString(linkConfiguration.error());
        return false;
    }

    setName(linkConfiguration.name());

    _hostAddress = QHostAddress(linkConfiguration.args()->at(0));
    _port = linkConfiguration.args()->at(1).toInt();

    return true;
}

bool UDPLink::finishConnection()
{
    _udpSocket->close();
    return true;
}

UDPLink::~UDPLink()
{
    finishConnection();
}
