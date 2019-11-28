#pragma once

#include "simulationlink.h"
#include <QTimer>

/**
 * @brief Link that simulates Ping sensor behaviour
 *
 */
class Ping360SimulationLink : public SimulationLink {
public:
    /**
     * @brief Construct a new Ping1D Simulation Link object
     *
     * @param parent
     */
    Ping360SimulationLink(QObject* parent = nullptr);

    /**
     * @brief Generates random data
     *
     */
    void randomUpdate();

    /**
     * @brief Handle incoming data from the sensor class
     *
     * @param byteArray
     */
    void handleData(const QByteArray& byteArray);

    /**
     * @brief Check if connection is writable
     *
     * @return true
     * @return false
     */
    bool isWritable() override final { return true; };

private:
    int _counter;
};
