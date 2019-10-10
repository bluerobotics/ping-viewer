#pragma once

#include <QString>
#include <QVariant>

#include "sensorinfo.h"

/**
 * @brief This defines the structure of the header in the log file
 *          This structure provides useful information about:
 *              - The structure version.
 *              - PingViewer version and OS where the program is running
 *              - Sensor information
 *
 */
struct LogSensorStruct {
    QString header; // Identification header
    uint version; // Log version number, should be increased for each change in this structure

    bool initialized = false; // Check if structure was already initialized

    /**
     * @brief Provide information about the PingViewer that wrote the log file
     *
     */
    struct PingViewerBuildInfo {
        QString hash;
        QString date;
        QString tag;
        QString osName;
        QString osVersion;
    } pingViewerBuildInfo;

    /**
     * @brief Provide information about the sensor family and type
     *
     */
    SensorInfo sensor;

    /**
     * @brief Initialize the structure with the basic information.
     *          This same function will be called by others if necessary.
     *          All values here should be valid for the `isValid` function.
     */
    void init()
    {
        header = _validHeader;
        version = _actualVersion;
        pingViewerBuildInfo.hash = QStringLiteral(GIT_VERSION);
        pingViewerBuildInfo.date = QStringLiteral(GIT_VERSION_DATE);
        pingViewerBuildInfo.tag = QStringLiteral(GIT_TAG);
        pingViewerBuildInfo.osName = QSysInfo::prettyProductName();
        pingViewerBuildInfo.osVersion = QSysInfo::productVersion();
        initialized = true;
    }

    /**
     * @brief Check if structure is valid
     *
     * @return true
     * @return false
     */
    bool isValid()
    {
        // The only values that should be checked are the ones that does not change between versions
        return header == _validHeader && (version > 0 && version <= _actualVersion);
    }

    /**
     * @brief Set the sensor information
     *
     * @param sensorInfo
     */
    void setSensorInfo(SensorInfo sensorInfo)
    {
        if (!initialized) {
            init();
        }

        sensor = sensorInfo;
    }

    uint _actualVersion = 1;
    QString _validHeader = QStringLiteral("PingViewer sensor log file");
};

/**
 * @brief Helper function for LogSensorStruct and QDataStream
 *
 * @param out
 * @param other
 * @return QDataStream&
 */
QDataStream& operator<<(QDataStream& out, const LogSensorStruct& other);

/**
 * @brief Helper function for LogSensorStruct and QDataStream
 *
 * @param in
 * @param other
 * @return QDataStream&
 */
QDataStream& operator>>(QDataStream& in, LogSensorStruct& other);

/**
 * @brief Helper function to print LogSensorStruct in a human friendly way
 *
 * @param out
 * @param other
 * @return QDebug
 */
QDebug operator<<(QDebug out, const LogSensorStruct& other);
