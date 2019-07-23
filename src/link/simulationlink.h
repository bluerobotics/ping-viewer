#pragma once

#include "abstractlink.h"
#include "parser-ping.h"

/**
 * @brief Simulation connection class
 *
 */
class SimulationLink : public AbstractLink
{
public:
    /**
     * @brief Construct a new Simulation Link object
     *
     * @param parent
     */
    SimulationLink(QObject* parent = nullptr);

    /**
     * @brief Check if connection is writable
     *
     * @return true
     * @return false
     */
    bool isWritable() final { return false; }
};
