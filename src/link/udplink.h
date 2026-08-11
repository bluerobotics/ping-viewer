#pragma once

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
    bool isOpen() final { return _udpSocket->isWritable() && _udpSocket->isReadable(); };

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
    bool startConnection() final { return _udpSocket->open(QIODevice::ReadWrite); };

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
     * @brief Reset the failure counter and reconnect backoff after a successful connection.
     *
     */
    void resetConnectionState();

    /**
     * @brief Human friendly description of the socket, used in the error messages
     *
     * @return QString
     */
    QString socketDescription() const;

    QString _hostAddress;
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
