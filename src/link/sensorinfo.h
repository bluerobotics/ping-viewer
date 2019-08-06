#pragma once

#include "ping-message-all.h"

/**
 * @brief Define all possible families that ping-viewer can deal with
 *          TODO: We maybe need a better place for this
 */
enum class SensorFamily : int {
    UNKNOWN = 0,
    PING,
};

/**
 * @brief Provide information about the sensor family and type
 *
 */
struct SensorInfo {
    SensorFamily family; // Check what kind of family this sensor is this

    // TODO: add firmware version ?

    /**
     * The union will hold what kind of device is this.
     * Since the sensor type for a specific family will be an enum,
     * the union will be used to translate the same variable between multiple types,
     * resulting in a much safer and generic approach.
     */
    union Type {
        int value;
        PingDeviceType ping;
    } type;
};

/**
 * @brief Helper function to print SensorFamily in a human friendly way
 *
 * @param out
 * @param other
 * @return QDebug
 */
QDebug operator<<(QDebug out, const SensorFamily& other);

/**
 * @brief Helper function for SensorFamily and QDataStream
 *
 * @param out
 * @param other
 * @return QDataStream&
 */
QDataStream& operator<<(QDataStream& out, const SensorFamily& other);

/**
 * @brief Helper function for SensorFamily and QDataStream
 *
 * @param in
 * @param other
 * @return QDataStream&
 */
QDataStream& operator>>(QDataStream& in, SensorFamily& other);

/**
 * @brief Helper function to print LogSensorStruct::SensorInfo in a human friendly way
 *
 * @param out
 * @param other
 * @return QDebug
 */
QDebug operator<<(QDebug out, const SensorInfo& other);
