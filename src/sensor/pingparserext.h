#pragma once

#include "parser.h"
#include "ping-parser.h"

/**
 * @brief The PingParserExt class wraps the PingParser class from the ping-protocol submodule
 * and Extends it with signalling in order to subclass our Parser class
 */
class PingParserExt : public Parser {
public:
    /**
     * @brief Any messages parsed must be shorter than the buffer length
     */
    PingParserExt()
        : _parser(10240)
    {
    }

    /**
     * @brief clear parse state
     */
    void clearBuffer() override final;

    /**
     * @brief asynchronous use, Child classes should signal when something happens ie. 'emit newMessage(Message m)'
     * @param data the next sequence of bytes in the serial stream being parsed
     */
    void parseBuffer(const QByteArray& data) override final;

    /**
     * @brief synchronous use, Child should return flags indicating incremental parse result/status
     * @param byte the next byte in serial stream being parsed
     * @return the current state of the parser
     */
    ParserState parseByte(const char byte) override final;

private:
    PingParser _parser;
};
