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
        emit newData(_udpSocket->readAll());
    });

    connect(this, &AbstractLink::sendData, this, [this](const QByteArray& data) {
        _udpSocket->write(data);
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

    // Check protocol detector comments and documentation about correct connect procedure
    // TODO: Maybe UDPLink should provide a static function unify server connection/test

    // Bind port
    _udpSocket->bind(QHostAddress::Any, _port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    // Connect with server
    _udpSocket->connectToHost(_hostAddress, _port);

    // Give the socket a second to connect to the other side otherwise error out
    int socketAttemps = 0;
    while(!_udpSocket->waitForConnected(100) && socketAttemps < 10 ) {
        // Increases socketAttemps here to avoid empty loop optimization
        socketAttemps++;
    }
    if(_udpSocket->state() != QUdpSocket::ConnectedState) {
        qCWarning(PING_PROTOCOL_UDPLINK) << "Socket is not in connected state.";
        QString errorMessage = QStringLiteral("Error (%1): %2.").arg(_udpSocket->state()).arg(_udpSocket->errorString());
        qCWarning(PING_PROTOCOL_UDPLINK) << errorMessage;
        return false;
    }

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
