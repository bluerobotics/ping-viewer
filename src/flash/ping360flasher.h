#pragma once

#include "flasher.h"
#include "ping360flashworker.h"

/**
 * @brief Flasher implementation for Ping360 sensor device
 *
 */
class Ping360Flasher : public Flasher {
    Q_OBJECT
public:
    /**
     * @brief Construct a new Ping360Flasher object
     * @param parent
     */
    Ping360Flasher(QObject* parent);

    void flash() override final;

private:
    Ping360FlashWorker _worker;
};
