#include <QDataStream>
#include <QDebug>
#include <QSysInfo>

#include "logsensorstruct.h"

QDataStream& operator<<(QDataStream& out, const LogSensorStruct& other)
{
    // Header
    out << other.header << other.version;

    // PingViewer
    out << other.pingViewerBuildInfo.hash;
    out << other.pingViewerBuildInfo.date;
    out << other.pingViewerBuildInfo.tag;
    out << other.pingViewerBuildInfo.osName;
    out << other.pingViewerBuildInfo.osVersion;

    // Sensor
    out << other.sensor.family;
    out << other.sensor.type.value;
    return out;
}

QDataStream& operator>>(QDataStream& in, LogSensorStruct& other)
{
    // Header
    in >> other.header >> other.version;

    // Ping viewer
    in >> other.pingViewerBuildInfo.hash;
    in >> other.pingViewerBuildInfo.date;
    in >> other.pingViewerBuildInfo.tag;
    in >> other.pingViewerBuildInfo.osName;
    in >> other.pingViewerBuildInfo.osVersion;

    // Sensor
    in >> other.sensor.family;
    in >> other.sensor.type.value;
    return in;
}

QDebug operator<<(QDebug out, const LogSensorStruct& other)
{
    // Header
    out << "Log file:" << other.header << "Version:" << other.version << "\n";

    // PingViewer
    out << "PingViewer build information:\n";
    out << "\t Hash:" << other.pingViewerBuildInfo.hash << "\n";
    out << "\t Date:" << other.pingViewerBuildInfo.date << "\n";
    out << "\t Tag:" << other.pingViewerBuildInfo.tag << "\n";
    // TODO: Send a PR for Qt moving name to a static function
    out << "\t OS:" << other.pingViewerBuildInfo.osName << other.pingViewerBuildInfo.osVersion << "\n";

    // Sensor
    out << "Sensor family and type:" << other.sensor << "\n";
    return out;
}
