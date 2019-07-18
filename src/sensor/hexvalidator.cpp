#include <QFile>
#include <QLoggingCategory>

#include "hexvalidator.h"

Q_LOGGING_CATEGORY(HEX_VALIDATOR, "ping.hexvalidator")

bool HexValidator::check(const QByteArray& bytes)
{
    // Line Format:
    //      :AABBBBCCnXDD
    // AA: Byte count
    // BBBB: Address
    // CC: Record type
    // nX: n characters, where n is AA*2
    //  - If AA is 3, nX will be: 0102FF
    // DD: Checksum byte

    auto error = [&bytes](const char* text, ...) {
        va_list args;
        va_start(args, text);
        qCWarning(HEX_VALIDATOR) << "In line:" << bytes;
        qCWarning(HEX_VALIDATOR) << QString::vasprintf(text, args);
        va_end(args);
        return false;
    };

    // Check if start with :
    if(bytes[0] != ':') {
        return error("No valid start key.");
    }

    // Check if it has an odd number of elements
    if(bytes.size()%2 == 0) {
        return error("Number of elements is not odd: %d", bytes.size());
    }

    // Get all values as normal bytes
    bool valid = false;
    QList<uint8_t> uintBytes;
    int numberOfBytesInLine = (bytes.size() - 1)/2; // Remove (:) character
    for(int i = 0; i < numberOfBytesInLine; i++) {
        uint8_t value = QString(bytes.mid(1 + i*2, 2)).toUInt(&valid, 16);
        uintBytes.append(value);
        if(!valid) {
            return error("No valid hexadecimal value.");
        }
    }

    // Check if byteCount has the correct size
    int byteCount = uintBytes[0];
    // (:) 1, (Byte Count) 2, (Address) 4, (Record type) 2, (Checksum) 2
    int calcSize = (bytes.size() - 1 - 2 - 4 - 2 - 2)/2;
    if(byteCount != calcSize) {
        return error("No valid byteCount: (%d) does not match with line size (%d).", byteCount,  calcSize);
    }

    // Calculate checksum and compare it
    uint8_t calculatedCheckSum = 0;
    // Checksum is the last value
    for(int i = 0, end = uintBytes.size() - 1; i < end; i++) {
        calculatedCheckSum += uintBytes[i];
    }
    // Do the two's complement
    calculatedCheckSum = ~calculatedCheckSum + 1;

    uint8_t checkSum = uintBytes.last();
    if(calculatedCheckSum != checkSum) {
        return error("No valid checksum: Calculated CS (%d) does not match (%d).", calculatedCheckSum,  checkSum);
    }

    return true;
}

bool HexValidator::isValidFile(const QString& fileUrl)
{
    QFile file(fileUrl);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(HEX_VALIDATOR) << "Not possible to open the file:" << fileUrl;
        return false;
    }

    while(!file.atEnd()) {
        // Read each line and remove \n
        QByteArray line = file.readLine().replace('\n', QByteArray());
        if(!check(line)) {
            return false;
        }
    }
    file.close();

    return true;
};
