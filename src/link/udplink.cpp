#include <QDebug>
#include <QLoggingCategory>
#include <QNetworkDatagram>

#include "logger.h"
#include "networkmanager.h"
#include "udplink.h"

PING_LOGGING_CATEGORY(PING_PROTOCOL_UDPLINK, "ping.protocol.udplink")

UDPLink::UDPLink(QObject* parent)
    : AbstractLink("UDPLink", parent)
    , _udpSocket(new QUdpSocket(this))
{
    setType(LinkType::Udp);

    connect(_udpSocket, &QIODevice::readyRead, this, [this] {
        resetConnectionState();
        emit newData(_udpSocket->readAll());
    });
    connect(_udpSocket, &QAbstractSocket::errorOccurred, this,
        [this](QAbstractSocket::SocketError /*socketError*/) { printErrorMessage(); });

    // QUdpSocket fail to emit state signal
    // Here we use a timer to check if the socket can still talk with the host, if not we try again
    connect(&_stateTimer, &QTimer::timeout, this, [this] {
        // The timer runs from the construction, there is no host to reach before setConfiguration
        if (_hostAddress.isEmpty() || isSocketUsable()) {
            return;
        }

        handleConnectionFailure();
        qCDebug(PING_PROTOCOL_UDPLINK) << "Trying to reconnect with host again.";
        reconnect();

        // Back off the reconnect interval so we stop flooding the log and network while the
        // target stays unreachable, capping at _maxReconnectIntervalMs.
        _stateTimer.setInterval(qMin(_stateTimer.interval() * 2, _maxReconnectIntervalMs));
    });
    _stateTimer.start(_baseReconnectIntervalMs);

    connect(this, &AbstractLink::sendData, this, [this](const QByteArray& data) {
        if (_udpSocket->write(data) == data.size()) {
            // A write that reaches the socket layer says nothing about the other side being there,
            // only received data does, so the failure counters are not touched here.
            _writeErrorReported = false;
            return;
        }

        // Qt discards the datagram without any error signal when the socket layer is gone, the
        // message is printed once per failure streak to not flood the log with a single dead link.
        if (!_writeErrorReported) {
            _writeErrorReported = true;
            qCWarning(PING_PROTOCOL_UDPLINK)
                << "Fail to write" << data.size() << "bytes in" << _linkConfiguration << socketDescription();
        }
    });
}

bool UDPLink::setConfiguration(const LinkConfiguration& linkConfiguration)
{
    _linkConfiguration = linkConfiguration;
    qCDebug(PING_PROTOCOL_UDPLINK) << linkConfiguration;
    if (!linkConfiguration.isValid()) {
        qCDebug(PING_PROTOCOL_UDPLINK) << LinkConfiguration::errorToString(linkConfiguration.error());

        // We allow wrong subnet connections
        // A VPN software or something may be using OS API to provice access to it
        if (linkConfiguration.error() != LinkConfiguration::Error::InvalidSubnet) {
            return false;
        }
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

    if (!isSocketUsable()) {
        printErrorMessage();
        return false;
    }

    return true;
}

void UDPLink::printErrorMessage()
{
    qCWarning(PING_PROTOCOL_UDPLINK) << "An error has occurred with:" << _linkConfiguration;
    QString errorMessage = QStringLiteral("Error (%1): %2.").arg(_udpSocket->error()).arg(_udpSocket->errorString());
    qCWarning(PING_PROTOCOL_UDPLINK) << errorMessage << socketDescription();
}

QString UDPLink::socketDescription() const
{
    return QStringLiteral("Socket state: %1, descriptor: %2")
        .arg(_udpSocket->state())
        .arg(_udpSocket->socketDescriptor());
}

bool UDPLink::isSocketUsable() const
{
    // A connected UDP socket can lose the descriptor without any state change or error signal, an
    // ICMP error for a datagram already sent is enough to do it. Checking the state alone reports
    // such a socket as healthy forever while every write is silently dropped.
    return _udpSocket->state() == QAbstractSocket::ConnectedState && _udpSocket->socketDescriptor() != -1;
}

void UDPLink::reconnect()
{
    // The socket still describes itself as connected, connectToHost alone would keep the dead
    // descriptor. abort() drops the socket layer so a new one is created.
    _udpSocket->abort();
    _udpSocket->connectToHost(_hostAddress, _port);
}

void UDPLink::handleConnectionFailure()
{
    printErrorMessage();
    _connectionErrorCount++;

    // Escalate only once, with an actionable message, instead of silently retrying forever.
    if (!_errorEscalated && _connectionErrorCount >= _errorEscalationThreshold) {
        _errorEscalated = true;

        QString message = QStringLiteral("Unable to reach the device at %1:%2.").arg(_hostAddress).arg(_port);
        if (!NetworkManager::isAddressInSubnet(_hostAddress)) {
            message += QStringLiteral(" It is on a different network than your computer. Set the device IP "
                                      "or your computer's network settings so they share a subnet.");
        }
        qCWarning(PING_PROTOCOL_UDPLINK) << message;
        emit linkError(message);
    }
}

void UDPLink::resetConnectionState()
{
    // Called on every datagram, restarting the timer here on a healthy link is pure waste
    if (_connectionErrorCount == 0 && !_errorEscalated && !_writeErrorReported
        && _stateTimer.interval() == _baseReconnectIntervalMs) {
        return;
    }

    _connectionErrorCount = 0;
    _errorEscalated = false;
    _writeErrorReported = false;
    _stateTimer.setInterval(_baseReconnectIntervalMs);
}

bool UDPLink::finishConnection()
{
    _udpSocket->close();
    return true;
}

UDPLink::~UDPLink() { finishConnection(); }
