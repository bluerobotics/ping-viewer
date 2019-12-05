#include <QDebug>
#include <QLoggingCategory>
#include <QNetworkDatagram>

#include "logger.h"
#include "udplink.h"

PING_LOGGING_CATEGORY(PING_PROTOCOL_UDPLINK, "ping.protocol.udplink")

UDPLink::UDPLink(QObject* parent)
    : AbstractLink("UDPLink", parent)
    , _udpSocket(new QUdpSocket(parent))
{
    setType(LinkType::Udp);

    connect(_udpSocket, &QIODevice::readyRead, this, [this] { emit newData(_udpSocket->readAll()); });
    connect(_udpSocket, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this,
        [this](QAbstractSocket::SocketError /*socketError*/) { printErrorMessage(); });

    // QUdpSocket fail to emit state signal
    // Here we use a timer to check if we are in a connect state, if not we try again
    connect(&_stateTimer, &QTimer::timeout, this, [this] {
        if (_udpSocket->state() == QAbstractSocket::UnconnectedState) {
            printErrorMessage();
            qDebug(PING_PROTOCOL_UDPLINK) << "Trying to connect with host again.";
            _udpSocket->connectToHost(_hostAddress, _port);
        }
    });
    _stateTimer.start(1000);

    connect(this, &AbstractLink::sendData, this, [this](const QByteArray& data) { _udpSocket->write(data); });
}

bool UDPLink::setConfiguration(const LinkConfiguration& linkConfiguration)
{
    _linkConfiguration = linkConfiguration;
    qCDebug(PING_PROTOCOL_UDPLINK) << linkConfiguration;
    if (!linkConfiguration.isValid()) {
        qCDebug(PING_PROTOCOL_UDPLINK) << LinkConfiguration::errorToString(linkConfiguration.error());
        return false;
    }

    setName(linkConfiguration.name());

    _hostAddress = linkConfiguration.args()->at(0);
    _port = linkConfiguration.args()->at(1).toInt();

    // Check protocol detector comments and documentation about correct connect procedure

    // Connect with server
    _udpSocket->connectToHost(_hostAddress, _port);

    // Give the socket a second to connect to the other side otherwise error out
    int socketAttemps = 0;
    while (!_udpSocket->waitForConnected(100) && socketAttemps < 10) {
        // Increases socketAttemps here to avoid empty loop optimization
        socketAttemps++;
    }

    if (_udpSocket->state() != QUdpSocket::ConnectedState) {
        printErrorMessage();
        return false;
    }

    return true;
}

void UDPLink::printErrorMessage()
{
    qCWarning(PING_PROTOCOL_UDPLINK) << "An error has occurred with:" << _linkConfiguration;
    QString errorMessage = QStringLiteral("Error (%1): %2.").arg(_udpSocket->state()).arg(_udpSocket->errorString());
    qCWarning(PING_PROTOCOL_UDPLINK) << errorMessage;
}

bool UDPLink::finishConnection()
{
    _udpSocket->close();
    return true;
}

UDPLink::~UDPLink() { finishConnection(); }
