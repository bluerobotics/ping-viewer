#include <QCoreApplication>
#include <QElapsedTimer>
#include <QtMath>

#include "ping-message-ping360.h"
#include "ping360simulationlink.h"

Ping360SimulationLink::Ping360SimulationLink(QObject* parent)
    : SimulationLink(parent)
    , _counter(0)
    , _globalAverageTimeMs(0)
    , _spins(0)
{
    _elapsedTimer.start();
    connect(this, &AbstractLink::sendData, this, &Ping360SimulationLink::handleData, Qt::QueuedConnection);
}

void Ping360SimulationLink::handleData(const QByteArray& byteArray)
{
    ping_message message(reinterpret_cast<const uint8_t*>(byteArray.data()), byteArray.size());
    if (message.message_id() == CommonId::GENERAL_REQUEST) {
        auto generalRequest = *static_cast<const common_general_request*>(&message);
        if (generalRequest.requested_id() == CommonId::DEVICE_INFORMATION) {
            common_device_information deviceInformation;
            deviceInformation.updateChecksum();
            emit newData(QByteArray(
                reinterpret_cast<const char*>(deviceInformation.msgData), deviceInformation.msgDataLength()));
        }
    } else if (message.message_id() == Ping360Id::TRANSDUCER) {
        randomUpdate();
    }
}

void Ping360SimulationLink::randomUpdate()
{
    static const float numberOfSamples = 1200;
    static const int angularResolution = 400;

    const float stop1 = numberOfSamples / 2.0 - 10 * qSin(_counter / 10.0);
    const float stop2 = 3 * numberOfSamples / 5.0 + 6 * qCos(_counter / 5.5);

    static ping360_device_data deviceData(numberOfSamples);
    deviceData.set_mode(0);
    deviceData.set_gain_setting(1);
    deviceData.set_angle(_counter % angularResolution);
    deviceData.set_transmit_duration(1000);
    deviceData.set_sample_period(80);
    deviceData.set_transmit_frequency(700);
    deviceData.set_number_of_samples(numberOfSamples);
    deviceData.set_data_length(numberOfSamples);

    for (int i = 0; i < numberOfSamples; i++) {
        float point;
        if (i < stop1) {
            point = 0.1 * randomPoint<uint8_t>();
        } else if (i < stop2) {
            point
                = 255 * ((-4.0 / qPow((stop2 - stop1), 2.0)) * qPow((i - stop1 - ((stop2 - stop1) / 2.0)), 2.0) + 1.0);
        } else {
            point = 0.45 * randomPoint<uint8_t>();
        }
        deviceData.set_data_at(i, point);
    }

    deviceData.updateChecksum();
    emit newData(QByteArray(reinterpret_cast<const char*>(deviceData.msgData), deviceData.msgDataLength()));

    // Calculate the global average time between requests
    _counter++;
    float elapsedTime = _elapsedTimer.elapsed();
    float weight = (_counter - 1) / static_cast<float>(_counter);
    _globalAverageTimeMs = _globalAverageTimeMs * weight + elapsedTime * (1 - weight);

    _elapsedTimer.restart();

    // This should be done after _counter increase to not interate before a full first spin
    if (_counter % angularResolution == 0) {
        _spins++;
    }

#if defined(PING360_SPEED_TEST)
    if (_spins > simulationTest.spins) {
        // Since we are forcing the application to close, qDebug may not work
        printf("%s: Average elapsed request elapsed time %f [Valid: %f]\n", __PRETTY_FUNCTION__, _globalAverageTimeMs,
            simulationTest.maxAverageRequestTimeMs);

        if (_globalAverageTimeMs > simulationTest.maxAverageRequestTimeMs) {
            QCoreApplication::exit(-1);
        } else {
            QCoreApplication::exit(0);
        }
    }
#endif
}
