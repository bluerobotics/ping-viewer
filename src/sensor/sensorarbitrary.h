#pragma once

#include <QJsonObject>
#include <QVariant>

#include "sensor.h"

/// A class to handle a single arbitrary key:value
/// arriving in the form of plain text Json
class SensorArbitrary : public Sensor
{
    Q_OBJECT
public:
    SensorArbitrary();

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

    void handleJsonObject(const QJsonObject& obj);
};
