#include "ping360bootloaderpacket.h"

uint16_t Ping360BootloaderPacket::packet_get_payload_length(packet_t packet) { return *(uint16_t*)(packet + 2); }

uint16_t Ping360BootloaderPacket::packet_get_length(packet_t packet) { return packet_get_payload_length(packet) + 7; }

uint8_t Ping360BootloaderPacket::packet_calculate_checksum(packet_t packet)
{
    uint8_t checksum = packet[1] ^ packet[2];
    checksum = checksum ^ packet[3];
    for (int i = 0; i < packet_get_payload_length(packet); i++) {
        checksum ^= packet[4 + i];
    }
    return checksum;
}

Ping360BootloaderPacket::packet_id_e Ping360BootloaderPacket::packet_get_id(packet_t packet)
{
    return static_cast<Ping360BootloaderPacket::packet_id_e>(packet[1]);
}

uint8_t Ping360BootloaderPacket::packet_calculate_complement(packet_t packet) { return ~packet_get_id(packet) + 1; }

void Ping360BootloaderPacket::packet_update_footer(packet_t packet)
{
    uint8_t* checksum_p = packet + 4 + packet_get_payload_length(packet);
    *checksum_p = packet_calculate_checksum(packet);
    *(checksum_p + 1) = packet_calculate_complement(packet);
}

Ping360BootloaderPacket::packet_parse_state_e Ping360BootloaderPacket::packet_parse_byte(const uint8_t byte)
{
    // don't overflow the parse buffer
    if (parser.rxTail >= PACKET_MAX_LENGTH) {
        return ERROR;
    }

    switch (parser.parseState) {
    case ERROR:
    case NEW_MESSAGE:
    case WAIT_START:
        parser.packetLength = 0;
        parser.rxTail = 0;
        if (byte != PACKET_FRAMING_START) {
            parser.parseState = ERROR;
        } else {
            parser.rxBuffer[parser.rxTail++] = byte;
            parser.parseState = WAIT_ID;
        }
        break;
    case WAIT_ID:
        if (byte < ID_LOW || byte >= ID_HIGH) {
            parser.parseState = ERROR;
        } else {
            parser.rxBuffer[parser.rxTail++] = byte;
            parser.parseState = WAIT_LENGTH_L;
        }
        break;
    case WAIT_LENGTH_L:
        parser.rxBuffer[parser.rxTail++] = byte;
        parser.parseState = WAIT_LENGTH_H;
        break;
    case WAIT_LENGTH_H:
        parser.rxBuffer[parser.rxTail++] = byte;
        parser.packetLength = packet_get_payload_length(parser.rxBuffer);
        if (parser.packetLength > PACKET_MAX_LENGTH) {
            parser.parseState = ERROR;
        } else if (parser.packetLength == 0) {
            parser.parseState = WAIT_CHECKSUM;
        } else {
            parser.parseState = WAIT_DATA;
        }
        break;
    case WAIT_DATA:
        parser.rxBuffer[parser.rxTail++] = byte;
        parser.packetLength--;
        if (!parser.packetLength) {
            parser.parseState = WAIT_CHECKSUM;
        }
        break;
    case WAIT_CHECKSUM:
        if (byte != packet_calculate_checksum(parser.rxBuffer)) {
            parser.parseState = ERROR;
        } else {
            parser.rxBuffer[parser.rxTail++] = byte;
            parser.parseState = WAIT_COMPLEMENT;
        }
        break;
    case WAIT_COMPLEMENT:
        if (byte != packet_calculate_complement(parser.rxBuffer)) {
            parser.parseState = ERROR;
        } else {
            parser.rxBuffer[parser.rxTail++] = byte;
            parser.parseState = WAIT_END;
        }
        break;
    case WAIT_END:
        if (byte != PACKET_FRAMING_END) {
            parser.parseState = ERROR;
        } else {
            parser.rxBuffer[parser.rxTail++] = byte;
            parser.parseState = NEW_MESSAGE;
        }
        break;
    default:
        parser.parseState = ERROR;
        break;
    }
    return parser.parseState;
}
