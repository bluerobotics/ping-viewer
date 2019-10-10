#pragma once

/**
 * @brief Validate .hex files
 *  For more information:
 *    https://en.wikipedia.org/wiki/Intel_HEX
 *    http://easyonlineconverter.com/converters/checksum_converter.html
 *
 */
class HexValidator {
public:
    /**
     * @brief Construct a new Hex Validator object
     *
     */
    HexValidator() = delete;

    /**
     * @brief Destroy the Hex Validator object
     *
     */
    ~HexValidator() = delete;

    /**
     * @brief Check if line is valid
     *  Format: :AABBBBCCnXDD
     *  Remember to remove \n or any other special character
     *
     * @param bytes
     * @return true
     * @return false
     */
    static bool check(const QByteArray& bytes);

    /**
     * @brief Validate a Intel Hex file
     *
     * @param fileUrl
     * @return true
     * @return false
     */
    static bool isValidFile(const QString& fileUrl);
};
