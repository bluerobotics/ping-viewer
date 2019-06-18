#include <QtMath>

#include "ping1dsimulationlink.h"
#include <ping-message-ping1d.h>

Ping1DSimulationLink::Ping1DSimulationLink(QObject* parent)
    : SimulationLink(parent)
{
    connect(&_randomUpdateTimer, &QTimer::timeout, this, &Ping1DSimulationLink::randomUpdate);
    _randomUpdateTimer.start(50);
}

void Ping1DSimulationLink::randomUpdate()
{
    static uint counter = 1;
    static const float numPoints = 200;
    static const float maxDepth = 70000;
    const float stop1 = numPoints / 2.0 - 10 * qSin(counter / 10.0);
    const float stop2 = 3 * numPoints / 5.0 + 6 * qCos(counter / 5.5);
    const float osc =  maxDepth*(1.3 + qCos(counter / 40.0)) / 2.3;

    uint8_t conf = 400 / (stop2 - stop1);

    static ping1d_profile profile(numPoints);

    profile.set_distance(osc*(stop2+stop1)/(numPoints*2));
    profile.set_confidence(conf);
    profile.set_transmit_duration(200);
    profile.set_ping_number(counter);
    profile.set_scan_start(0);
    profile.set_scan_length(osc);
    profile.set_gain_setting(4);
    profile.set_profile_data_length(numPoints);

    for (int i = 0; i < numPoints; i++) {
        float point;
        if (i < stop1) {
            point = 0.1 * (qrand()%256);
        } else if (i < stop2) {
            point = 255 * ((-4.0 / qPow((stop2-stop1), 2.0)) * qPow((i - stop1 - ((stop2-stop1) / 2.0)), 2.0)  + 1.0);
        } else {
            point = 0.45 * (qrand()%256);
        }
        profile.set_profile_data_at(i, point);
    }

    profile.updateChecksum();
    emit newData(QByteArray(reinterpret_cast<const char*>(profile.msgData), profile.msgDataLength()));

    counter++;
}
