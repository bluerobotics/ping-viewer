#pragma once

#include <QTimer>
#include "simulationlink.h"

/**
 * @brief Link that simulates Ping sensor behaviour
 *
 */
class PingSimulationLink : public SimulationLink
{
public:
    /**
     * @brief Construct a new Ping Simulation Link object
     *
     * @param parent
     */
    PingSimulationLink(QObject* parent = nullptr);

    /**
     * @brief Generates random data
     *
     */
    void randomUpdate();

private:
    QTimer _randomUpdateTimer;
};
