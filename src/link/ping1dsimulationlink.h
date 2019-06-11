#pragma once

#include <QTimer>
#include "simulationlink.h"

/**
 * @brief Link that simulates Ping sensor behaviour
 *
 */
class Ping1DSimulationLink : public SimulationLink
{
public:
    /**
     * @brief Construct a new Ping1D Simulation Link object
     *
     * @param parent
     */
    Ping1DSimulationLink(QObject* parent = nullptr);

    /**
     * @brief Generates random data
     *
     */
    void randomUpdate();

private:
    QTimer _randomUpdateTimer;
};
