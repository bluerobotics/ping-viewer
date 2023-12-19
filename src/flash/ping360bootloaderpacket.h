#pragma once

#include <inttypes.h>

/**
 * @brief A class to provide packet definitions, and to
 * facilitate creating and parsing packets
 */
class Ping360BootloaderPacket {
public:
    static const uint16_t PACKET_MAX_LENGTH = 1600;

    // the data length for read/write program memory
    static const uint16_t PACKET_ROW_LENGTH = 1536;

    /**
     * @brief packet framing bytes
     * every packet begins with PACKET_FRAMING_START
     * and ends with PACKET_FRAMING_END
     */
    typedef enum {
        PACKET_FRAMING_START = 0x5a,
        PACKET_FRAMING_END = 0xa5,
    } packet_framing_e;

    /**
     * @brief packet IDs
     * a packet is either a command or a response
     */
    typedef enum {
        ID_LOW = 0x30,
        RSP_NACK = 0x30,
        RSP_ACK = 0x31,
        RSP_PGM_MEM = 0x32,
        RSP_DEV_ID = 0x33,
        RSP_VERSION = 0x34,

        CMD_READ_DEV_ID = 0x41,
        CMD_READ_PGM_MEM = 0x42,
        CMD_WRITE_PGM_MEM = 0x43,
        CMD_WRITE_CFG_MEM = 0x44,
        CMD_RESET_PROCESSOR = 0x45,
        CMD_JUMP_START = 0x46,
        CMD_RESTART_BOOTLOADER = 0x47,
        CMD_READ_VERSION = 0x48,
        ID_HIGH
    } packet_id_e;

    /**
     * @brief a packet is a sequence of bytes
     */
    typedef uint8_t* packet_t;

    /**
     * @brief packet headers consist of:
     * - a start byte: 0x5A
     * - a byte indicating the command or response ID
     * - an unsigned 16 bit number indicating the packet payload length
     */
    typedef struct {
        uint8_t start;
        uint8_t id;
        uint16_t length;
    } packet_header_t;

    /**
     * @brief packet footers consist of:
     * - a checksum byte: the xor of bytes between
     * start and checksum non-inclusive
     * - a complement byte: 2's complement of command or response byte 1
     * - a terminator byte: 0xA5
     */
    typedef struct {
        uint8_t checksum;
        uint8_t complement;
        uint8_t end;
    } packet_footer_t;

    /**
     * @brief empty packets consist of a header and a footer
     * with no payload
     */
    typedef union {
        struct {
            packet_header_t header;
            packet_footer_t footer;
        } message;
        uint8_t data[7];
    } packet_empty_t;

    // empty packets (no payload) //

    /**
     * @brief command to read the device ID: CMD_READ_DEV_ID
     */
    typedef packet_empty_t packet_cmd_read_dev_id_t;

    /**
     * @brief command to reset the processor: CMD_RESET_PROCESSOR
     */
    typedef packet_empty_t packet_cmd_reset_processor_t;

    /**
     * @brief command to jump to the start of the application firmware: CMD_JUMP_START
     */
    typedef packet_empty_t packet_cmd_jump_start_t;

    /**
     * @brief command to restart the bootloader: CMD_RESTART_BOOTLOADER
     */
    typedef packet_empty_t packet_cmd_restart_bootloader_t;

    /**
     * @brief command to read the bootloader version: CMD_READ_VERSION
     */
    typedef packet_empty_t packet_cmd_read_version_t;

    /**
     * @brief a Not ACKnowledged response: RSP_NACK
     */
    typedef packet_empty_t packet_rsp_nack_t;

    /**
     * @brief an ACKnowledged response: RSP_ACK
     */
    typedef packet_empty_t packet_rsp_ack_t;

    static constexpr packet_cmd_read_dev_id_t packet_cmd_read_dev_id_init = {{
        {
            PACKET_FRAMING_START, CMD_READ_DEV_ID,
            0 // length
        }, // header
        {0, 0, PACKET_FRAMING_END} // footer
    }};

    static constexpr packet_cmd_reset_processor_t packet_cmd_reset_processor_init = {{
        {
            PACKET_FRAMING_START, CMD_RESET_PROCESSOR,
            0 // length
        }, // header
        {0, 0, PACKET_FRAMING_END} // footer
    }};

    static constexpr packet_cmd_jump_start_t packet_cmd_jump_start_init = {{
        {
            PACKET_FRAMING_START, CMD_JUMP_START,
            0 // length
        }, // header
        {0, 0, PACKET_FRAMING_END} // footer
    }};

    static constexpr packet_cmd_restart_bootloader_t packet_cmd_restart_bootloader_init = {{
        {
            PACKET_FRAMING_START, CMD_RESTART_BOOTLOADER,
            0 // length
        }, // header
        {0, 0, PACKET_FRAMING_END} // footer
    }};

    static constexpr packet_cmd_read_version_t packet_cmd_read_version_init = {
        {
            {
                PACKET_FRAMING_START, CMD_READ_VERSION,
                0 // length
            }, // header
            {0, 0, PACKET_FRAMING_END} // footer
        },
    };

    /**
     * @brief command to read the program memory region
     */
    typedef union {
        struct {
            packet_header_t header;
            uint32_t address;
            packet_footer_t footer;
        } message;
        uint8_t data[4 + 7];
    } packet_cmd_read_pgm_mem_t;

    static constexpr packet_cmd_read_pgm_mem_t packet_cmd_read_pgm_mem_init = {{
        {
            PACKET_FRAMING_START, CMD_READ_PGM_MEM,
            4 // length
        }, // header
        0, // address
        {0, 0, PACKET_FRAMING_END} // footer
    }};

    /**
     * @brief command to write the program memory region
     */
    typedef union {
        struct {
            packet_header_t header;
            uint32_t address;
            uint8_t rowData[PACKET_ROW_LENGTH];
            packet_footer_t footer;
        } message;
        uint8_t data[4 + PACKET_ROW_LENGTH + 7];
    } packet_cmd_write_pgm_mem_t;

    static constexpr packet_cmd_write_pgm_mem_t packet_cmd_write_pgm_mem_init = {{
        {
            PACKET_FRAMING_START, CMD_WRITE_PGM_MEM,
            4 + PACKET_ROW_LENGTH // length
        }, // header
        0, // address
        {0}, {0, 0, PACKET_FRAMING_END} // footer
    }};

    /**
     * @brief command to write the configuration memory region
     */
    typedef union {
        struct {
            packet_header_t header;
            uint8_t cfgData[24];
            packet_footer_t footer;
        } message;
        uint8_t data[24 + 7];
    } packet_cmd_write_cfg_mem_t;

    static constexpr packet_cmd_write_cfg_mem_t packet_cmd_write_cfg_mem_init = {{
        {
            PACKET_FRAMING_START, CMD_WRITE_CFG_MEM,
            24 // length
        }, // header
        {0}, {0, 0, PACKET_FRAMING_END} // footer
    }};

    /**
     * @brief a response to the CMD_READ_PGM_MEM command
     */
    typedef union {
        struct {
            packet_header_t header;
            uint8_t rowData[PACKET_ROW_LENGTH];
            packet_footer_t footer;
        } message;
        uint8_t data[PACKET_ROW_LENGTH + 7];
    } packet_rsp_pgm_mem_t;

    /**
     * @brief a response to the CMD_READ_DEV_ID command
     */
    typedef union {
        struct {
            packet_header_t header;
            uint16_t deviceId;
            packet_footer_t footer;
        } message;
        uint8_t data[2 + 7];
    } packet_rsp_dev_id_t;

    /**
     * @brief a response to the CMD_READ_VERSION command
     */
    typedef union {
        struct {
            packet_header_t header;
            uint8_t device_type;
            uint8_t device_revision;
            uint8_t version_major;
            uint8_t version_minor;
            uint8_t version_patch;
            packet_footer_t footer;
        } message;
        uint8_t data[5 + 7];
    } packet_rsp_version_t;

    /**
     * @brief states for parsing a packet
     */
    typedef enum {
        ERROR,
        WAIT_START,
        WAIT_ID,
        WAIT_LENGTH_L,
        WAIT_LENGTH_H,
        WAIT_DATA,
        WAIT_CHECKSUM,
        WAIT_COMPLEMENT,
        WAIT_END,
        NEW_MESSAGE,
    } packet_parse_state_e;

    /**
     * @brief a struct for parsing a packet with an internal
     * buffer and parse state
     */
    typedef struct packet_parser {
        packet_parse_state_e parseState;
        uint16_t rxTail;
        uint16_t packetLength;
        uint8_t rxBuffer[PACKET_MAX_LENGTH];
    } packet_parser_t;

    packet_parser_t parser = {WAIT_START};

    /**
     * @brief reset the parser to start parsing a new packet
     */
    void reset() { parser.parseState = WAIT_START; };

    /**
     * @brief parse a byte to form a ping360 bootloader packet
     * use reset() to start parsing a new packet
     * @param byte
     * @return NEW_MESSAGE if the byte completes a ping360 bootloader packet
     */
    packet_parse_state_e packet_parse_byte(const uint8_t byte);

    /**
     * @brief get the packet id for a packet
     * @param packet
     * @return the packet id
     */
    static packet_id_e packet_get_id(packet_t packet);

    /**
     * @brief get the length in bytes of the entire packet, header and footer included
     * @param packet
     * @return the packet data length in bytes
     */
    static uint16_t packet_get_length(packet_t packet);

    /**
     * @brief update the packet footer data with the calculated complement and checksum
     * @param packet
     */
    static void packet_update_footer(packet_t packet);

private:
    static uint16_t packet_get_payload_length(packet_t packet);
    static uint8_t packet_calculate_checksum(packet_t packet);
    static uint8_t packet_calculate_complement(packet_t packet);
};
