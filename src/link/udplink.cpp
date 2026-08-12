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

    connect(_udpSocket, &QIODevice::readyRead, this, &UDPLink::readPendingDatagrams);
    connect(_udpSocket, &QAbstractSocket::errorOccurred, this,
        [this](QAbstractSocket::SocketError /*socketError*/) { printErrorMessage(); });

    // QUdpSocket fail to emit state signal
    // Here we use a timer to check if the socket can still be used, if not we bind it again
    connect(&_stateTimer, &QTimer::timeout, this, [this] {
        // The timer runs from the construction, there is no host to reach before setConfiguration
        if (_hostAddress.isNull() || isSocketUsable()) {
            return;
        }

        handleConnectionFailure();
        qCDebug(PING_PROTOCOL_UDPLINK) << "Trying to bind the socket again.";
        bindSocket();

        // Back off the reconnect interval so we stop flooding the log and network while the
        // target stays unreachable, capping at _maxReconnectIntervalMs.
        _stateTimer.setInterval(qMin(_stateTimer.interval() * 2, _maxReconnectIntervalMs));
    });
    _stateTimer.start(_baseReconnectIntervalMs);

    connect(this, &AbstractLink::sendData, this, [this](const QByteArray& data) {
        if (_udpSocket->writeDatagram(data, _hostAddress, _port) == data.size()) {
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

    // The configuration also takes names, like `localhost` or `raspberrypi`
    _hostAddress = NetworkManager::addressToIp(linkConfiguration.args()->at(0));
    _port = linkConfiguration.args()->at(1).toInt();

    if (_hostAddress.isNull()) {
        qCWarning(PING_PROTOCOL_UDPLINK) << "Fail to resolve host:" << linkConfiguration.args()->at(0);
        return false;
    }

    return bindSocket();
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
    // A UDP socket can lose the descriptor without any state change or error signal, an ICMP error
    // for a datagram already sent is enough to do it. Checking the state alone reports such a
    // socket as healthy forever while every write is silently dropped.
    return _udpSocket->state() == QAbstractSocket::BoundState && _udpSocket->socketDescriptor() != -1;
}

bool UDPLink::bindSocket()
{
    // The sensor is not reached with connectToHost: macOS takes the descriptor of a connected UDP
    // socket away as soon as the first datagram goes to a local network address, and everything
    // written after it is dropped inside Qt. A bound socket with an explicit destination in each
    // datagram does the same job and is the way the Ping360 discovery already talks to the sensor.
    _udpSocket->abort();

    // MSVC also reads a SpecialAddress as a port, the explicit types keep the bind overload clear
    const quint16 randomPort = 0; // Force OS to give a random port
    if (!_udpSocket->bind(QHostAddress(QHostAddress::AnyIPv4), randomPort)) {
        printErrorMessage();
        return false;
    }

    return true;
}

void UDPLink::readPendingDatagrams()
{
    while (_udpSocket->hasPendingDatagrams()) {
        const QNetworkDatagram datagram = _udpSocket->receiveDatagram();

        // Windows reports an ICMP port unreachable as a failed read, there is nothing to deliver and
        // asking the same socket again would spin forever
        if (!datagram.isValid()) {
            break;
        }

        resetConnectionState();
        emit newData(datagram.data());
    }
}

void UDPLink::handleConnectionFailure()
{
    printErrorMessage();
    _connectionErrorCount++;

    // Escalate only once, with an actionable message, instead of silently retrying forever.
    if (!_errorEscalated && _connectionErrorCount >= _errorEscalationThreshold) {
        _errorEscalated = true;

        QString message
            = QStringLiteral("Unable to reach the device at %1:%2.").arg(_hostAddress.toString()).arg(_port);
        if (!NetworkManager::isAddressInSubnet(_hostAddress.toString())) {
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
