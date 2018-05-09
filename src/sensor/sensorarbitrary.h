#pragma once

#include <QJsonObject>
#include <QVariant>

#include "parsers/parser_json.h"
#include "sensor.h"

/// A class to handle a single arbitrary key:value
/// arriving in the form of plain text Json
class SensorArbitrary : public Sensor
{
    Q_OBJECT
public:

    SensorArbitrary() : Sensor() {
        _parser = new JsonParser();
        connect(dynamic_cast<JsonParser*>(_parser), &JsonParser::newJsonObject, this, &SensorArbitrary::handleJsonObject);
        connect(link(), &AbstractLink::newData, _parser, &Parser::parseBuffer);
    }

    Q_PROPERTY(QVariant value READ value NOTIFY valueUpdate)
    QVariant value() { return _value; }
    Q_PROPERTY(QString name READ name NOTIFY nameUpdate)
    QString name() { return _name; }

signals:
    void valueUpdate(QVariant value);
    void nameUpdate(QString name);

private:

    // TODO maybe QMap<QString, QVariant>
    QString _name;
    QVariant _value;

    void handleJsonObject(const QJsonObject& obj) {
        _name = (obj.begin().key());
        emit nameUpdate(_name);
        _value = (obj.begin().value().toVariant());
        emit valueUpdate(_value);
    }
};
