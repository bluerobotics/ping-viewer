#pragma once

#include <QSerialPort>

#include "abstractlink.h"

/**
 * @brief Serial Link connection class
 *
 */
class SerialLink : public AbstractLink
{
public:
    /**
     * @brief Construct a new Serial Link object
     *
     * @param parent
     */
    SerialLink(QObject* parent = nullptr);

    /**
     * @brief Destroy the Serial Link object
     *
     */
    ~SerialLink();

    /**
     * @brief Human friendly error string
     *
     * @return QString
     */
    QString errorString() const final { return _port.errorString(); }

    /**
     * @brief Finish connection
     *
     * @return true
     * @return false
     */
    bool finishConnection() final;

    /**
     * @brief Check if connection is open
     *
     * @return true
     * @return false
     */
    bool isOpen() const final { return _port.isWritable() && _port.isReadable(); }

    /**
     * @brief Return a list of all available connections
     *
     * @return QStringList
     */
    QStringList listAvailableConnections() final;

    /**
     * @brief Return a list of available ports
     *
     * @return QSerialPort*
     */
    QSerialPort* port() { return &_port; }

    /**
     * @brief Set link configuration
     *
     * @param linkConfiguration
     * @return true
     * @return false
     */
    bool setConfiguration(const LinkConfiguration& linkConfiguration) final;

    /**
     * @brief Start connection and check if is ready to go
     *
     * @return true
     * @return false
     */
    bool startConnection() final override;

private:
    QSerialPort _port;
    QStringList _availableConnections;
};
