#pragma once

#include <QProcess>
#include <QSharedPointer>
#include <QTimer>

#include "sensor.h"
#include "parsers/parser.h"
#include "parsers/parser_ping.h"
#include "parsers/detector.h"
#include "pingmessage/pingmessage_all.h"

class Ping : public Sensor
{
    Q_OBJECT
public:

    Ping();
    ~Ping() { _detector.terminate(); _detector.wait(); }

    void connectLink(const QString& connString);
    void printStatus(); // console debug

    Q_PROPERTY(int srcId READ srcId NOTIFY srcIdUpdate)
    uint8_t srcId() { return _srcId; }

    Q_PROPERTY(int dstId READ dstId NOTIFY dstIdUpdate)
    uint8_t dstId() { return _dstId; }

    Q_PROPERTY(int fw_version_major READ fw_version_major NOTIFY fwVersionMajorUpdate)
    uint16_t fw_version_major() { return _fw_version_major; }

    Q_PROPERTY(int fw_version_minor READ fw_version_minor NOTIFY fwVersionMinorUpdate)
    uint16_t fw_version_minor() { return _fw_version_minor; }

    Q_PROPERTY(int device_type READ device_type NOTIFY deviceTypeUpdate)
    uint8_t device_type() { return _device_type; }

    Q_PROPERTY(int device_model READ device_model NOTIFY deviceModelUpdate)
    uint8_t device_model() { return _device_model; }

    Q_PROPERTY(int distance READ distance NOTIFY distanceUpdate)
    uint32_t distance() { return _distance; }

    Q_PROPERTY(int ping_number READ ping_number NOTIFY pingNumberUpdate)
    uint32_t ping_number() { return _ping_number; }

    Q_PROPERTY(int confidence READ confidence NOTIFY confidenceUpdate)
    uint8_t confidence() { return _confidence; }

    Q_PROPERTY(int pulse_usec READ pulse_usec NOTIFY pulseUsecUpdate)
    uint16_t pulse_usec() { return _pulse_usec; }

    Q_PROPERTY(int start_mm READ start_mm NOTIFY startMmUpdate)
    uint32_t start_mm() { return _start_mm; }

    Q_PROPERTY(int length_mm READ length_mm NOTIFY lengthMmUpdate)
    uint32_t length_mm() { return _length_mm; }

    Q_PROPERTY(int gain_index READ gain_index NOTIFY gainIndexUpdate)
    uint32_t gain_index() { return _gain_index; }

    Q_PROPERTY(QList<double> points READ points NOTIFY pointsUpdate)
    QList<double> points() { return _points; }

    Q_PROPERTY(bool mode_auto READ mode_auto NOTIFY modeAutoUpdate)
    bool mode_auto() { return _mode_auto; }

    Q_PROPERTY(int msec_per_ping READ msec_per_ping WRITE set_msec_per_ping NOTIFY msecPerPingUpdate)
    uint16_t msec_per_ping() { return _msec_per_ping; }
    void set_msec_per_ping(uint16_t msec_per_ping)
    {
        ping_msg_ping1D_set_ping_rate_msec m;
        m.set_rate_msec(msec_per_ping);
        m.updateChecksum();
        writeMessage(m);
        request(PingMessage::ping1D_ping_rate_msec);
    }

    Q_PROPERTY(QVariant pollFrequency READ pollFrequency WRITE setPollFrequency NOTIFY pollFrequencyUpdate)
    QVariant pollFrequency();
    void setPollFrequency(QVariant pollFrequency);

    // TODO, maybe store history/filtered history of values in this
    // object for access by different visual elements without need to recompute
    // TODO install filters here?

    Q_INVOKABLE void request(int id); // send request for message

    Q_INVOKABLE void firmwareUpdate(QString fileUrl, bool sendPingGotoBootloader = true);

signals:
    void srcIdUpdate();
    void dstIdUpdate();
    void deviceTypeUpdate();
    void deviceModelUpdate();
    void fwVersionMajorUpdate();
    void fwVersionMinorUpdate();

    void distanceUpdate();
    void pingNumberUpdate();
    void confidenceUpdate();
    void pulseUsecUpdate();
    void startMmUpdate();
    void lengthMmUpdate();
    void gainIndexUpdate();
    void pointsUpdate();

    void modeAutoUpdate();
    void msecPerPingUpdate();

    void flashProgress(float progress);
    void flashComplete();

    void pollFrequencyUpdate();
private:
    uint8_t _srcId;
    uint8_t _dstId;

    uint8_t _device_type;
    uint8_t _device_model;
    uint16_t _fw_version_major;
    uint16_t _fw_version_minor;

    uint32_t _distance; // mm
    uint8_t _confidence; // 0-100%
    uint16_t _pulse_usec;
    uint32_t _ping_number;
    uint32_t _start_mm;
    uint32_t _length_mm;
    uint32_t _gain_index;

    float _fw_update_perc;

    static const uint16_t _num_points = 200;

    // TODO maybe use vector or uint8_t[] here
    // QVector is only required if points need to be exposed to qml
    //QVector<int> _points;
    QList<double> _points;

    bool _mode_auto;
    uint16_t _msec_per_ping;

    // TODO const &
    void handleMessage(PingMessage msg); // handle incoming message
    void writeMessage(const PingMessage& msg); // write a messge to link

    void firmwareUpdatePercentage();
    void flash(const QString& portLocation, const QString& firmwareFile);

    ProtocolDetector _detector;
    QTimer _requestTimer;

    QSharedPointer<QProcess> _firmwareProcess;
};
