#include "parser-ping.h"

void PingParserExt::clearBuffer()
{
    _parser.reset();
}

void PingParserExt::parseBuffer(const QByteArray& data)
{
    for(int i = 0; i < data.length(); i++) {
        PingParser::ParseState state = _parser.parseByte(data.at(i));
        if (state == PingParser::ParseState::NEW_MESSAGE) {
            parsed++;
            _rxMessage = _parser.rxMessage;
            emit newMessage(_rxMessage);
        } else if (state == PingParser::ParseState::ERROR) {
            errors++;
            emit parseError();
        }
    }
}

Parser::ParserState PingParserExt::parseByte(const char byte)
{
    PingParser::ParseState state = _parser.parseByte(byte);
    switch(state) {
    case PingParser::ParseState::NEW_MESSAGE:
        _rxMessage = _parser.rxMessage;
        return Parser::ParserState::NEW_MESSAGE;
    case PingParser::ParseState::ERROR:
        return Parser::ParserState::ERROR;
    case PingParser::ParseState::WAIT_START :
    case PingParser::ParseState::WAIT_HEADER :
    case PingParser::ParseState::WAIT_LENGTH_L:
    case PingParser::ParseState::WAIT_LENGTH_H:
    case PingParser::ParseState::WAIT_MSG_ID_L:
    case PingParser::ParseState::WAIT_MSG_ID_H:
    case PingParser::ParseState::WAIT_SRC_ID:
    case PingParser::ParseState::WAIT_DST_ID:
    case PingParser::ParseState::WAIT_PAYLOAD:
    case PingParser::ParseState::WAIT_CHECKSUM_L:
    case PingParser::ParseState::WAIT_CHECKSUM_H:
        return Parser::ParserState::PARSING;
    }
    assert("Should never trigger");
    return Parser::ParserState::PARSING;
}
