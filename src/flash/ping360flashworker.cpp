#include "ping360flashworker.h"
#include "logger.h"
#include "pic-hex.h"

#include <QByteArray>
#include <QSerialPortInfo>
#include <QThread>

PING_LOGGING_CATEGORY(PING360FLASHWORKER, "ping360.flash.worker")

Ping360FlashWorker::Ping360FlashWorker()
    : QThread()
{
}

void Ping360FlashWorker::run()
{
    // The flash progress is divided into two, or three process phases depending
    // on if _verify is true:
    // 1. Erase
    // 2. Flash
    // 3. Verify (optional)
    // the flash progress percent factor is 100 divided by the number of process phases
    float flashProgressPercentFactor = _verify ? 33.3f : 50.0f;
    emit stateChanged(Flasher::StartingFlash);

    _port = new QSerialPort();
    QSerialPortInfo pInfo(_link.serialPort());
    _port->setPort(pInfo);
    _port->setBaudRate(_baudRate);

    for (int i = 0; i < 10; i++) {
        QThread::msleep(10);
        if (_port->open(QIODevice::ReadWrite)) {
            break;
        }
    }

    if (!_port->isOpen()) {
        error("error opening port");
        return;
    }

    qCInfo(PING360FLASHWORKER) << "fetch device id...";
    uint16_t id = 0;
    bool id_read = false;
    for (int i = 0; i < 10; i++) {
        QThread::msleep(10);
        if (bl_read_device_id(&id)) {
            id_read = true;
            break;
        }
    }

    if (id_read) {
        qCInfo(PING360FLASHWORKER) << QString::asprintf(" > device id: 0x%04x <\n", id);
    } else {
        error("error fetching device id");
        return;
    }

    qCInfo(PING360FLASHWORKER) << "fetch version...";
    Ping360BootloaderPacket::packet_rsp_version_t version;
    if (bl_read_version(&version)) {
        if (version.message.version_major != _expectedVersionMajor
            || version.message.version_minor != _expectedVersionMinor
            || version.message.version_patch != _expectedVersionPatch) {
            error(QString::asprintf("error, bootloader version is v%d.%d.%d, expected v%d.%d.%d",
                version.message.version_major, version.message.version_minor, version.message.version_patch,
                _expectedVersionMajor, _expectedVersionMinor, _expectedVersionPatch));
            return;
        }

        qCInfo(PING360FLASHWORKER) << QString::asprintf(
            " > device type 0x%02x : hardware revision %c : bootloader v%d.%d.%d <\n", version.message.device_type,
            version.message.device_revision, version.message.version_major, version.message.version_minor,
            version.message.version_patch);

    } else {
        error("error fetching version");
        return;
    }

    qCInfo(PING360FLASHWORKER) << QString::asprintf(
        "loading ping360 firmware from %s...", _firmwareFilePath.toLocal8Bit().data());

    // PicHex hex = PicHex(_firmwareFilePath.toLocal8Bit().data());
    PicHex hex = PicHex();
    hex.pic_hex_read_file(_firmwareFilePath.toLocal8Bit().constData());
    emit stateChanged(Flasher::Flashing);

    qCInfo(PING360FLASHWORKER) << "erasing program memory";

    uint8_t fill[Ping360BootloaderPacket::PACKET_ROW_LENGTH];
    memset(fill, 0xff, sizeof(fill));
    for (int i = 0; i < 86; i++) {
        if (i >= 1 && i <= 3) {
            continue;
        }
        if (bl_write_program_memory(fill, i * 0x400)) {
            qCInfo(PING360FLASHWORKER) << QString("erase memory address 0x%1...ok").arg(i * 0x400, 8, 16, QChar('0'));
        } else {
            error(QString("error erasing memory address 0x%1").arg(i * 0x400, 8, 16, QChar('0')));
            return;
        }
        emit flashProgressChanged(flashProgressPercentFactor * i / 86.0f);
    }
    qCInfo(PING360FLASHWORKER) << "writing application...";
    for (int i = 0; i < 86; i++) {
        if (i >= 1 && i <= 3) {
            continue; // protected boot code
        }
        if (i == 0) {
            continue; // we write this page last, to prevent booting after failed programming
        }

        if (bl_write_program_memory(
                hex.pic_hex_application_data + i * Ping360BootloaderPacket::PACKET_ROW_LENGTH, i * 0x400)) {
            qCInfo(PING360FLASHWORKER) << QString("write 0x%1: ok").arg(i * 0x400, 8, 16, QChar('0'));
        } else {
            error(QString("write memory address 0x%1: error").arg(i * 0x400, 8, 16, QChar('0')));
            return;
        }
        emit flashProgressChanged(flashProgressPercentFactor + flashProgressPercentFactor * i / 86.0f);
    }

    uint16_t bootAddress = 0x0000;
    if (bl_write_program_memory(hex.pic_hex_application_data, bootAddress)) {
        qCInfo(PING360FLASHWORKER) << QString("write boot memory address 0x%1...").arg(bootAddress, 8, 16, QChar('0'))
                                   << "ok";
    } else {
        error(QString("error writing boot memory address 0x%1...").arg(bootAddress, 8, 16, QChar('0')));
        return;
    }

    if (_verify) {
        qCInfo(PING360FLASHWORKER) << "verifying application...";
        uint8_t* verify;
        for (int i = 0; i < 86; i++) {
            if (i >= 1 && i <= 3) {
                continue; // protected bootloader code
            }
            uint32_t offset = i * 0x400;
            bool verify_ok = true;
            if (bl_read_program_memory(&verify, offset)) {
                for (int j = 0; j < Ping360BootloaderPacket::PACKET_ROW_LENGTH; j++) {
                    if (verify[j] != hex.pic_hex_application_data[i * Ping360BootloaderPacket::PACKET_ROW_LENGTH + j]) {
                        qCWarning(PING360FLASHWORKER)
                            << QString::asprintf("error: program data differs at 0x%08llx: 0x%02x != 0x%02x\n",
                                   i * Ping360BootloaderPacket::PACKET_ROW_LENGTH + j, verify[j],
                                   hex.pic_hex_application_data[i * Ping360BootloaderPacket::PACKET_ROW_LENGTH + j]);
                    }
                }
                qCInfo(PING360FLASHWORKER) << QString::asprintf("verify 0x%08x: ok", offset);
            } else {
                error(QString::asprintf("verify 0x%08x: error reading program memory", offset));
                return;
            }
            emit flashProgressChanged(2 * flashProgressPercentFactor + flashProgressPercentFactor * i / 86.0f);
        }
    }

    if (bl_write_configuration_memory(hex.pic_hex_configuration_data)) {
        qCInfo(PING360FLASHWORKER) << "writing configuration...ok";
    } else {
        error("error writing configuration memory");
        return;
    }

    // flash is complete
    emit flashProgressChanged(100.0f);

    if (bl_reset()) {
        qCInfo(PING360FLASHWORKER) << "starting application...ok";
    } else {
        qCCritical(PING360FLASHWORKER) << "starting application...error";
        error("error starting pin360 main application");
        return;
    }

    _port->close();
    delete _port;

    emit stateChanged(Flasher::FlashFinished);
}

#define BL_TIMEOUT_DEFAULT_US 750000
#define BL_TIMEOUT_WRITE_US 500000
#define BL_TIMEOUT_READ_US 5000000

void Ping360FlashWorker::bl_write_packet(const packet_t packet)
{
    // for (int i = 0; i < bl_parser.packet_get_length(packet); i++) {
    //     qCCritical(PING360FLASHWORKER) << i << packet[i];
    // }
    port_write(packet, bl_parser.packet_get_length(packet));
}

Ping360BootloaderPacket::packet_t Ping360FlashWorker::bl_wait_packet(uint8_t id, uint32_t timeout_us)
{
    bl_parser.reset();

    uint64_t tstop = time_us() + timeout_us;
    // qCCritical(PING360FLASHWORKER) << "waiting for packet" << time_us() << tstop;

    uint8_t b;
    while (time_us() < tstop) {
        // QThread::msleep(10);
        _port->waitForReadyRead(10);
        for (int i = 0; i < _port->bytesAvailable(); i++) {
            if (port_read(&b, 1) > 0) {
                Ping360BootloaderPacket::packet_parse_state_e parseResult = bl_parser.packet_parse_byte(b);
                // qCInfo(PING360FLASHWORKER) << parseResult;
                if (parseResult == Ping360BootloaderPacket::NEW_MESSAGE) {
                    if (Ping360BootloaderPacket::packet_get_id(bl_parser.parser.rxBuffer) == id) {
                        return bl_parser.parser.rxBuffer;
                    } else {
                        printf("bootloader error: got unexpected id 0x%02x while waiting for 0x%02x",
                            Ping360BootloaderPacket::packet_get_id(bl_parser.parser.rxBuffer), id);
                        return NULL;
                    }
                } else if (parseResult == Ping360BootloaderPacket::ERROR) {
                    printf("bootloader error: parse error while waiting for 0x%02x!\n", id);
                    return NULL;
                }
            }
        }
    }
    printf("bootloader error: timed out waiting for 0x%02x!\n", id);
    return NULL;
}

// false on nack or error
bool Ping360FlashWorker::bl_read_device_id(uint16_t* device_id)
{
    Ping360BootloaderPacket::packet_cmd_read_dev_id_t readDevId = Ping360BootloaderPacket::packet_cmd_read_dev_id_init;
    Ping360BootloaderPacket::packet_update_footer(readDevId.data);
    bl_write_packet(readDevId.data);
    Ping360BootloaderPacket::packet_t ret = bl_wait_packet(Ping360BootloaderPacket::RSP_DEV_ID, BL_TIMEOUT_DEFAULT_US);
    if (ret) {
        Ping360BootloaderPacket::packet_rsp_dev_id_t* resp = (Ping360BootloaderPacket::packet_rsp_dev_id_t*)ret;
        *device_id = resp->message.deviceId;
        return true;
    }
    return false;
}

bool Ping360FlashWorker::bl_read_version(Ping360BootloaderPacket::packet_rsp_version_t* version)
{
    Ping360BootloaderPacket::packet_cmd_read_version_t readVersion
        = Ping360BootloaderPacket::packet_cmd_read_version_init;
    Ping360BootloaderPacket::packet_update_footer(readVersion.data);
    bl_write_packet(readVersion.data);
    Ping360BootloaderPacket::packet_t ret = bl_wait_packet(Ping360BootloaderPacket::RSP_VERSION, BL_TIMEOUT_DEFAULT_US);
    if (ret) {
        *version = *(Ping360BootloaderPacket::packet_rsp_version_t*)ret;
        return true;
    }
    return false;
}

// false on nack or error
bool Ping360FlashWorker::bl_read_program_memory(uint8_t** data, uint32_t address)
{
    Ping360BootloaderPacket::packet_cmd_read_pgm_mem_t pkt = Ping360BootloaderPacket::packet_cmd_read_pgm_mem_init;
    pkt.message.address = address;
    Ping360BootloaderPacket::packet_update_footer(pkt.data);
    bl_write_packet(pkt.data);
    Ping360BootloaderPacket::packet_t ret = bl_wait_packet(Ping360BootloaderPacket::RSP_PGM_MEM, BL_TIMEOUT_READ_US);
    if (ret) {
        Ping360BootloaderPacket::packet_rsp_pgm_mem_t* resp = (Ping360BootloaderPacket::packet_rsp_pgm_mem_t*)ret;
        uint8_t* rowData = resp->message.rowData;

        // rotate data to fix endianness (read is opposite endianness of write)
        for (uint16_t i = 0; i < 512; i++) {
            uint16_t idx = i * 3;
            uint8_t tmp = rowData[idx];
            rowData[idx] = rowData[idx + 2];
            rowData[idx + 2] = tmp;
        }

        *data = rowData;

        return true;
    }
    return false;
}

// false on nack or error
bool Ping360FlashWorker::bl_write_program_memory(const uint8_t* data, uint32_t address)
{
    Ping360BootloaderPacket::packet_cmd_write_pgm_mem_t pkt = Ping360BootloaderPacket::packet_cmd_write_pgm_mem_init;
    memcpy(pkt.message.rowData, data, Ping360BootloaderPacket::PACKET_ROW_LENGTH);
    pkt.message.address = address;
    Ping360BootloaderPacket::packet_update_footer(pkt.data);
    bl_write_packet(pkt.data);
    return bl_wait_packet(Ping360BootloaderPacket::RSP_ACK, BL_TIMEOUT_WRITE_US);
}

// false on nack or error
bool Ping360FlashWorker::bl_write_configuration_memory(const uint8_t* data)
{
    Ping360BootloaderPacket::packet_cmd_write_cfg_mem_t pkt = Ping360BootloaderPacket::packet_cmd_write_cfg_mem_init;
    memcpy(pkt.message.cfgData, data, 24);
    Ping360BootloaderPacket::packet_update_footer(pkt.data);
    bl_write_packet(pkt.data);
    return bl_wait_packet(Ping360BootloaderPacket::RSP_ACK, BL_TIMEOUT_DEFAULT_US);
}

bool Ping360FlashWorker::bl_reset()
{
    Ping360BootloaderPacket::packet_cmd_reset_processor_t pkt
        = Ping360BootloaderPacket::packet_cmd_reset_processor_init;
    Ping360BootloaderPacket::packet_update_footer(pkt.data);
    bl_write_packet(pkt.data);
    return bl_wait_packet(Ping360BootloaderPacket::RSP_ACK, BL_TIMEOUT_DEFAULT_US);
}

int Ping360FlashWorker::port_write(const uint8_t* buffer, int nBytes)
{
    // _link.self()->sendData(QByteArray(reinterpret_cast<const char*>(buffer), nBytes));
    int bytes = _port->write(reinterpret_cast<const char*>(buffer), nBytes);
    _port->flush();
    // if (!_port->waitForBytesWritten(1000)) {
    //   qCCritical(PING360FLASHWORKER) << "write timeout";
    // }
    // qCCritical(PING360FLASHWORKER) << "wrote" << bytes;
    return bytes;
}

int Ping360FlashWorker::port_read(uint8_t* data, int nBytes)
{
    int bytes = _port->read(reinterpret_cast<char*>(data), nBytes);
    // if (bytes > 0) qCCritical(PING360FLASHWORKER) << "read" << bytes << "/" << nBytes;
    return bytes;
}

void Ping360FlashWorker::error(const QString message)
{
    if (_port) {
        _port->close();
    }
    emit messageChanged(message);
    emit stateChanged(Flasher::Error);
}
