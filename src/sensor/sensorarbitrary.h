#pragma once

#include <QJsonObject>
#include <QVariant>

#include "sensor.h"

/**
 * @brief Handle a single arbitrary key:value
 * arriving in the form of plain text Json
 */
class SensorArbitrary : public Sensor
{
    Q_OBJECT
public:
    SensorArbitrary();

    /**
     * @brief Get value
     *
     * @return QVariant
     */
    QVariant value() { return _value; }
    Q_PROPERTY(QVariant value READ value NOTIFY valueUpdate)

    /**
     * @brief Get name
     *
     * @return QString
     */
    QString name() { return _name; }
    Q_PROPERTY(QString name READ name NOTIFY nameUpdate)

signals:
    void valueUpdate(QVariant value);
    void nameUpdate(QString name);

private:

    // TODO maybe QMap<QString, QVariant>
    QString _name;
    QVariant _value;

    void handleJsonObject(const QJsonObject& obj);
};
