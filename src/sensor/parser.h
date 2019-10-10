#pragma once

#include "ping-message.h"
#include <QObject>

/**
 * This class digests data and notifies owner when something interesting happens
 */
class Parser : public QObject {
    Q_OBJECT
public:
    Parser() = default;
    ~Parser() = default;

    /**
     * @brief parser states
     */
    enum ParserState {
        ERROR, // got some unexpected data
        PARSING, // got some expected data, feed me more
        NEW_MESSAGE // got a new packet
    };

    uint32_t parsed = 0; // number of messages/packets successfully parsed
    uint32_t errors = 0; // number of parse errors

    /**
     * @brief clear parse state
     */
    virtual void clearBuffer() = 0;

    /**
     * @brief asynchronous use, Child classes should signal when something happens ie. 'emit newMessage(Message m)'
     * @param data the next sequence of bytes in the serial stream being parsed
     */
    virtual void parseBuffer(const QByteArray& data) = 0;

    /**
     * @brief synchronous use, Child should return flags indicating incremental parse result/status
     * @param byte the next byte in serial stream being parsed
     * @return the current state of the parser
     */
    virtual ParserState parseByte(const char byte) = 0;

    /**
     * @brief Access the last message that was parsed for synchronous use
     *
     * @return the last message that was parsed
     */
    ping_message rxMessage() const { return _rxMessage; }

signals:
    void newMessage(const ping_message& msg);
    void parseError();

protected:
    ping_message _rxMessage;
};
