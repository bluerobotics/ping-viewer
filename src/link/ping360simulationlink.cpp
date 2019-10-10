#include <QtMath>

#include "ping-message-ping360.h"
#include "ping360simulationlink.h"

Ping360SimulationLink::Ping360SimulationLink(QObject* parent)
    : SimulationLink(parent)
{
    connect(&_randomUpdateTimer, &QTimer::timeout, this, &Ping360SimulationLink::randomUpdate);
    _randomUpdateTimer.start(50);
}

void Ping360SimulationLink::randomUpdate()
{
    static uint counter = 1;
    static const float numberOfSamples = 1200;
    static const int angularResolution = 400;

    const float stop1 = numberOfSamples / 2.0 - 10 * qSin(counter / 10.0);
    const float stop2 = 3 * numberOfSamples / 5.0 + 6 * qCos(counter / 5.5);

    static ping360_device_data deviceData(numberOfSamples);
    deviceData.set_mode(0);
    deviceData.set_gain_setting(1);
    deviceData.set_angle(counter % angularResolution);
    deviceData.set_transmit_duration(1000);
    deviceData.set_sample_period(80);
    deviceData.set_transmit_frequency(700);
    deviceData.set_number_of_samples(numberOfSamples);
    deviceData.set_data_length(numberOfSamples);

    for (int i = 0; i < numberOfSamples; i++) {
        float point;
        if (i < stop1) {
            point = 0.1 * (qrand() % 256);
        } else if (i < stop2) {
            point
                = 255 * ((-4.0 / qPow((stop2 - stop1), 2.0)) * qPow((i - stop1 - ((stop2 - stop1) / 2.0)), 2.0) + 1.0);
        } else {
            point = 0.45 * (qrand() % 256);
        }
        deviceData.set_data_at(i, point);
    }

    deviceData.updateChecksum();
    emit newData(QByteArray(reinterpret_cast<const char*>(deviceData.msgData), deviceData.msgDataLength()));

    counter++;
}
