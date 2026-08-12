#pragma once

#include <QHostAddress>
#include <QUdpSocket>

#include "abstractlink.h"

/**
 * @brief UDP connection class
 *
 */
class UDPLink : public AbstractLink {
public:
    /**
     * @brief Construct a new UDPLink object
     *
     * @param parent
     */
    UDPLink(QObject* parent = nullptr);

    /**
     * @brief Destroy the UDPLink object
     *
     */
    ~UDPLink();

    /**
     * @brief Return a human friendly error message
     *
     * @return QString
     */
    QString errorString() final { return _udpSocket->errorString(); };

    /**
     * @brief Finish connection
     *
     * @return true
     * @return false
     */
    bool finishConnection() final;

    /**
     * @brief Check if UDP connection is open
     *
     * @return true
     * @return false
     */
    bool isOpen() final { return isSocketUsable(); };

    /**
     * @brief Set the configuration object
     *
     * @param linkConfiguration
     * @return true
     * @return false
     */
    bool setConfiguration(const LinkConfiguration& linkConfiguration) final;

    /**
     * @brief Start connection
     *
     * @return true
     * @return false
     */
    bool startConnection() final { return isSocketUsable() || bindSocket(); };

    /**
     * @brief Return QUdpSocket pointer
     *
     * @return QUdpSocket*
     */
    QUdpSocket* udpSocket() { return _udpSocket; };

private:
    /**
     * @brief Function used internally to print debug information about the link
     *
     */
    void printErrorMessage();

    /**
     * @brief Handle a failed connection attempt: log it, count consecutive failures, and once the
     *  escalation threshold is reached emit a single actionable linkError instead of silently
     *  retrying forever.
     *
     */
    void handleConnectionFailure();

    /**
     * @brief Reset the failure counter and reconnect backoff after a successful transfer.
     *
     */
    void resetConnectionState();

    /**
     * @brief Check if the socket is able to transfer data. QAbstractSocket::state() is not enough,
     *  a UDP socket can be left without a valid descriptor while still reporting a valid state,
     *  silently dropping everything that is written on it.
     *
     * @return true
     * @return false
     */
    bool isSocketUsable() const;

    /**
     * @brief Drop the current socket layer and bind a new one to talk with the host
     *
     * @return true
     * @return false
     */
    bool bindSocket();

    /**
     * @brief Deliver the datagrams available in the socket
     *
     */
    void readPendingDatagrams();

    /**
     * @brief Human friendly description of the socket, used in the error messages
     *
     * @return QString
     */
    QString socketDescription() const;

    QHostAddress _hostAddress;
    QTimer _stateTimer;
    QUdpSocket* _udpSocket;
    uint _port;

    int _connectionErrorCount {0};
    bool _errorEscalated {false};
    bool _writeErrorReported {false};
    static constexpr int _baseReconnectIntervalMs {1000};
    static constexpr int _maxReconnectIntervalMs {8000};
    static constexpr int _errorEscalationThreshold {3};
};
