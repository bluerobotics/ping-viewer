#pragma once

#include <QSerialPort>

#include "abstractlink.h"

/**
 * @brief Serial Link connection class
 *
 */
class SerialLink : public AbstractLink {
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
     * @brief get the serial port baudrate
     * @return the serial port baudrate
     */
    qint32 getBaudRate() { return _port.baudRate(); }

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

    /**
     * @brief Set the serial port to work in low latency mode
     * This function was based in a series of links and documentations:
     *  - http://www.tekkotsu.org/dox/hal/SerialCommPort_8cc_source.html
     *  - https://www.pjrc.com/tmp/host_software/receive_test.c
     *  - https://opensource.apple.com/source/ntp/ntp-124/ntpd/refclock_parse.c
     *  - https://docs.microsoft.com/pt-br/windows/win32/api/winbase/nf-winbase-setcommtimeouts
     *  - https://docs.microsoft.com/pt-br/windows/win32/api/winbase/ns-winbase-commtimeouts
     *  - https://www.ftdichip.com/Support/Knowledgebase/index.html?settingacustomdefaultlaten.htm
     *
     * @return true
     * @return false
     */
    bool setLowLatency();

private:
    QSerialPort _port;
};
