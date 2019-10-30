#pragma once

#include "abstractlink.h"
#include "pingparserext.h"

#include <QRandomGenerator>

/**
 * @brief Simulation connection class
 *
 */
class SimulationLink : public AbstractLink {
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
    bool isWritable() final { return false; };

protected:
    /**
     * @brief Limit the random generation to the interval of max and min possible values of T
     *
     * @return float random value
     */
    template <typename T> float randomPoint() const
    {
        return QRandomGenerator::global()->bounded(std::numeric_limits<T>::max() + 1);
    };
};
