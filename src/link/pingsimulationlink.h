#pragma once

#include <QTimer>
#include "simulationlink.h"

class PingSimulationLink : public SimulationLink
{
public:
    PingSimulationLink(QObject* parent = nullptr);
    void randomUpdate();

    QTimer _randomUpdateTimer;
};
