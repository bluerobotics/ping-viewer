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
    QString errorString() final { return _port.errorString(); };

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
    bool isOpen() final { return _port.isWritable() && _port.isReadable(); };

    /**
     * @brief Return a list of all available connections
     *
     * @return QStringList
     */
    QStringList listAvailableConnections() final;

    /**
     * @brief Return a list of available ports
     * Any change in the port should be notified and dealed via `configurationChanged()`
     *
     * @return QSerialPort*
     */
    const QSerialPort* port() const { return &_port; };

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

    /**
     * @brief Set a valid baudrate
     *
     * @param baudRate
     */
    void setBaudRate(int baudRate);

    /**
     * @brief Force sensor to do automatic baud rate detection
     *
     */
    void forceSensorAutomaticBaudRateDetection();

    void writeSync(const char* data, int size) final override {
        _port.write(data, size);
        _port.flush();
    }

private:
    QSerialPort _port;
};
