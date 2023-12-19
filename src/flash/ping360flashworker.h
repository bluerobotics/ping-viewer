#pragma once

#include "flasher.h"
#include "ping360bootloaderpacket.h"

#include <QDateTime>
#include <QLoggingCategory>
#include <QSerialPort>
#include <QThread>

Q_DECLARE_LOGGING_CATEGORY(PING360FLASHWORKER)

/**
 * @brief Worker thread for flashing Ping360 devices
 *
 */
class Ping360FlashWorker : public QThread {
    Q_OBJECT
public:
    /**
     * @brief Construct a new Ping360FlashWorker object
     *
     */
    Ping360FlashWorker();

    /**
     * @brief Destroy the Ping360FlashWorker object
     *
     */
    ~Ping360FlashWorker() = default;

    /**
     * @brief Set the baud rate to be used for device communication
     *
     * @param baudRate
     *
     */
    void setBaudRate(int baudRate) { _baudRate = baudRate; }

    /**
     * @brief Set the path for the firmware hex file
     *
     * @param firmwareFilePath
     *
     */
    void setFirmwarePath(QString firmwareFilePath) { _firmwareFilePath = firmwareFilePath; }

    /**
     * @brief Set the link configuration to use for device communication
     *
     * @param link the link configuration to use for device communication
     *
     */
    void setLink(LinkConfiguration link) { _link = link; }

    /**
     * @brief Set the verify behavior of the flash process
     *
     * @param verify set to true to read back and verify the firmware after programming the device
     *
     */
    void setVerify(bool verify) { _verify = verify; }

    /**
     * @brief run the flashing process, overrides QThread::run
     */
    void run() override final;

signals:
    void messageChanged(QString message);
    void stateChanged(Flasher::States state);
    void flashProgressChanged(float flashPercent);

private:
    int _baudRate;
    QString _firmwareFilePath;
    LinkConfiguration _link;
    bool _verify = true;

    QSerialPort* _port;

    typedef Ping360BootloaderPacket::packet_t packet_t;
    void bl_write_packet(const packet_t packet);
    packet_t bl_wait_packet(uint8_t id, uint32_t timeout_us);

    bool bl_read_device_id(uint16_t* device_id);
    bool bl_read_version(Ping360BootloaderPacket::packet_rsp_version_t* version);
    bool bl_read_program_memory(uint8_t** data, uint32_t address);

    bool bl_write_program_memory(const uint8_t* data, uint32_t address);
    bool bl_write_configuration_memory(const uint8_t* data);

    bool bl_reset();

    static const uint16_t _expectedDeviceId = 0x062f;
    static const uint8_t _expectedVersionMajor = 2;
    static const uint8_t _expectedVersionMinor = 1;
    static const uint8_t _expectedVersionPatch = 2;

    Ping360BootloaderPacket bl_parser;

    qint64 time_us() { return QDateTime::currentMSecsSinceEpoch() * 1000; };
    int port_write(const uint8_t* buffer, int nBytes);
    int port_read(uint8_t* data, int nBytes);

    void error(QString message);
};
