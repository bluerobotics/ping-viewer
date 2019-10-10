#include <QDataStream>
#include <QDebug>

#include "sensorinfo.h"

QDebug operator<<(QDebug out, const SensorFamily& other)
{
    QString string;
    switch (other) {
    case SensorFamily::UNKNOWN:
        string = "UNKNOWN";
        break;
    case SensorFamily::PING:
        string = "PING";
        break;
    default:
        string = "INVALID";
        break;
    }
    return out << string;
}

QDataStream& operator<<(QDataStream& out, const SensorFamily& other) { return out << static_cast<int>(other); }

QDataStream& operator>>(QDataStream& in, SensorFamily& other)
{
    int value;
    in >> value;
    other = static_cast<SensorFamily>(value);
    return in;
}

QDebug operator<<(QDebug out, const SensorInfo& other)
{
    QString string;
    switch (other.family) {
    case SensorFamily::UNKNOWN:
        string = "UNKNOWN";
        break;
    case SensorFamily::PING:
        string = PingHelper::nameFromDeviceType(other.type.ping);
        break;
    default:
        string = "INVALID";
        break;
    }
    return out << other.family << string;
}
