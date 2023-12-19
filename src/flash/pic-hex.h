#pragma once

#include <cintelhex.h>

/**
 * @brief facilities to convert an intel hex to PIC microcontroller memory space
 */
class PicHex {
public:
    // 86 pages * 512 words * 3 bytes
    static const uint32_t PROGRAM_MEMORY_SIZE = 0x20400;
    static const uint32_t PROGRAM_MEMORY_OFFSET = 0x0;

    // 24 bytes configuration in separate memory region
    static const uint32_t CONFIGURATION_MEMORY_SIZE = 24;
    static const uint32_t CONFIGURATION_MEMORY_OFFSET = 0x00F80000;

    /**
     * @brief read a hex file and extract the firmware application and configuration data
     * @param filename
     * @return true if application and configuration data were successfully extracted
     */
    bool pic_hex_read_file(const char* filename);

    /**
     * @brief The hex file data for the application memory region
     * this data is valid if pic_hex_read_file returns true
     */
    uint8_t pic_hex_application_data[PROGRAM_MEMORY_SIZE];

    /**
     * @brief The hex file data for the configuration memory region
     * The configuration memory region is located at CONFIGURATION_MEMORY_OFFSET
     * and is CONFIGURATION_MEMORY_SIZE bytes wide
     * this data is valid if pic_hex_read_file returns true
     */
    uint8_t pic_hex_configuration_data[CONFIGURATION_MEMORY_SIZE];

private:
    bool pic_hex_mem_cpy(ihex_recordset_t* record_set, uint8_t* destination, uint32_t length, uint32_t offset);
    bool pic_hex_extract_application(const char* filename);
    bool pic_hex_extract_configuration(const char* filename);
};
