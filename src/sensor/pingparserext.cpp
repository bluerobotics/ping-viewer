#include "pingparserext.h"

void PingParserExt::clearBuffer() { _parser.reset(); }

void PingParserExt::parseBuffer(const QByteArray& data)
{
    for (int i = 0; i < data.length(); i++) {
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
    if (state == PingParser::ParseState::NEW_MESSAGE) {
        _rxMessage = _parser.rxMessage;
        return Parser::ParserState::NEW_MESSAGE;
    } else if (state == PingParser::ERROR) {
        return Parser::ParserState::ERROR;
    }

    return Parser::ParserState::PARSING;
}
