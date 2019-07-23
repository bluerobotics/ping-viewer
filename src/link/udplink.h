#pragma once

#include <QUdpSocket>

#include "abstractlink.h"

/**
 * @brief UDP connection class
 *
 */
class UDPLink : public AbstractLink
{
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
    QString errorString() const final { return _udpSocket->errorString(); }

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
    bool isOpen() const final { return _udpSocket->isWritable() && _udpSocket->isReadable(); }

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
    bool startConnection() final { return _udpSocket->open(QIODevice::ReadWrite); }

    /**
     * @brief Return QUdpSocket pointer
     *
     * @return QUdpSocket*
     */
    QUdpSocket* udpSocket() { return _udpSocket; }

private:
    QHostAddress _hostAddress;
    QUdpSocket* _udpSocket;
    uint16_t _port;
};
