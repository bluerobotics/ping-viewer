#include "sensorarbitrary.h"

#include "parsers/parser_json.h"

SensorArbitrary::SensorArbitrary()
    : _parser(new JsonParser())
{
    connect(dynamic_cast<JsonParser*>(_parser), &JsonParser::newJsonObject, this, &SensorArbitrary::handleJsonObject);
    connect(link(), &AbstractLink::newData, _parser, &Parser::parseBuffer);
}

void SensorArbitrary::handleJsonObject(const QJsonObject& obj)
{
    _name = (obj.begin().key());
    _value = (obj.begin().value().toVariant());
    emit nameUpdate(_name);
    emit valueUpdate(_value);
};