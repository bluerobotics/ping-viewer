#pragma once

#include <QProcess>
#include <QSharedPointer>
#include <QTimer>

#include "sensor.h"
#include "parsers/parser.h"
#include "parsers/parser_ping.h"
#include "parsers/detector.h"
#include "pingmessage/pingmessage_all.h"

/**
 * @brief Define ping sensor
 * 1D Sonar
 *
 */
class Ping : public Sensor
{
    Q_OBJECT
public:

    Ping();
    ~Ping() { _detector.terminate(); _detector.wait(); }

    /**
     * @brief Add connection
     *
     * @param connString Connection string defined as (int:string:arg)
     */
    Q_INVOKABLE void connectLink(const QStringList& connString);

    /**
     * @brief debug function
     */
    void printStatus();

    /**
     * @brief Get device source ID
     *
     * @return uint8_t source id
     */
    uint8_t srcId() { return _srcId; }
    Q_PROPERTY(int srcId READ srcId NOTIFY srcIdUpdate)

    /**
     * @brief Return destiny ID
     *
     * @return uint8_t destiny id
     */
    uint8_t dstId() { return _dstId; }
    Q_PROPERTY(int dstId READ dstId NOTIFY dstIdUpdate)

    /**
     * @brief Return firmware major version
     *
     * @return uint16_t firmware major version number
     */
    uint16_t fw_version_major() { return _fw_version_major; }
    Q_PROPERTY(int fw_version_major READ fw_version_major NOTIFY fwVersionMajorUpdate)

    /**
     * @brief Return firmware minor version
     *
     * @return uint16_t firmware minor version number
     */
    uint16_t fw_version_minor() { return _fw_version_minor; }
    Q_PROPERTY(int fw_version_minor READ fw_version_minor NOTIFY fwVersionMinorUpdate)

    /**
     * @brief Return device type number
     *
     * @return uint8_t Device type number
     */
    uint8_t device_type() { return _device_type; }
    Q_PROPERTY(int device_type READ device_type NOTIFY deviceTypeUpdate)

    /**
     * @brief Return device model number
     *
     * @return uint8_t Device model number
     */
    uint8_t device_model() { return _device_model; }
    Q_PROPERTY(int device_model READ device_model NOTIFY deviceModelUpdate)

    /**
     * @brief Return distance in mm
     *
     * @return uint32_t
     */
    uint32_t distance() { return _distance; }
    Q_PROPERTY(int distance READ distance NOTIFY distanceUpdate)

    /**
     * @brief Return number of pings emitted
     *
     * @return uint32_t
     */
    uint32_t ping_number() { return _ping_number; }
    Q_PROPERTY(int ping_number READ ping_number NOTIFY pingNumberUpdate)

    /**
     * @brief Return depth confidence
     *
     * @return uint8_t
     */
    uint8_t confidence() { return _confidence; }
    Q_PROPERTY(int confidence READ confidence NOTIFY confidenceUpdate)

    /**
     * @brief Return pulse emition in ms
     *
     * @return uint16_t
     */
    uint16_t pulse_usec() { return _pulse_usec; }
    Q_PROPERTY(int pulse_usec READ pulse_usec NOTIFY pulseUsecUpdate)

    /**
     * @brief Return start distance of sonar points in mm
     *
     * @return uint32_t
     */
    uint32_t start_mm() { return _start_mm; }

    /**
     * @brief Set sensor start analyze distance in mm
     *
     * @param start_mm
     */
    void set_start_mm(int start_mm)
    {
        ping_msg_ping1D_set_range m;
        m.set_start_mm(start_mm);
        m.set_length_mm(_length_mm);
        m.updateChecksum();
        writeMessage(m);
        request(Ping1DNamespace::Range);
    }
    Q_PROPERTY(int start_mm READ start_mm WRITE set_start_mm NOTIFY startMmUpdate)

    /**
     * @brief return points length in mm
     *
     * @return uint32_t
     */
    uint32_t length_mm() { return _length_mm; }

    /**
     * @brief Set sensor window analysis size
     *
     * @param length_mm
     */
    void set_length_mm(int length_mm)
    {
        ping_msg_ping1D_set_range m;
        m.set_start_mm(_start_mm);
        m.set_length_mm(length_mm);
        m.updateChecksum();
        writeMessage(m);
        request(Ping1DNamespace::Range);
    }
    Q_PROPERTY(int length_mm READ length_mm WRITE set_length_mm NOTIFY lengthMmUpdate)

    /**
     * @brief Return gain index
     *
     * @return uint32_t
     */
    uint32_t gain_index() { return _gain_index; }

    /**
     * @brief Set sensor gain index
     *
     * @param gain_index
     */
    void set_gain_index(int gain_index)
    {
        ping_msg_ping1D_set_gain_index m;
        m.set_index(gain_index);
        m.updateChecksum();
        writeMessage(m);
        request(Ping1DNamespace::Gain_index);
    }
    Q_PROPERTY(int gain_index READ gain_index WRITE set_gain_index NOTIFY gainIndexUpdate)

    /**
     * @brief Return last array of points
     *
     * @return QList<double>
     */
    QList<double> points() { return _points; }
    Q_PROPERTY(QList<double> points READ points NOTIFY pointsUpdate)

    /**
     * @brief Get auto mode status
     *
     * @return true running
     * @return false not running
     */
    bool mode_auto() { return _mode_auto; }

    /**
     * @brief Set sensor auto mode
     *
     * @param mode_auto
     */
    void set_mode_auto(bool mode_auto)
    {
        ping_msg_ping1D_set_auto_manual m;
        m.set_mode(mode_auto);
        m.updateChecksum();
        writeMessage(m);
        request(Ping1DNamespace::Mode);
    }
    Q_PROPERTY(bool mode_auto READ mode_auto WRITE set_mode_auto NOTIFY modeAutoUpdate)

    /**
     * @brief Get time between pings in ms
     *
     * @return uint16_t
     */
    uint16_t msec_per_ping() { return _msec_per_ping; }

    /**
     * @brief Set time between pings in ms
     *
     * @param msec_per_ping
     */
    void set_msec_per_ping(uint16_t msec_per_ping)
    {
        ping_msg_ping1D_set_ping_rate_msec m;
        m.set_rate_msec(msec_per_ping);
        m.updateChecksum();
        writeMessage(m);
        request(Ping1DNamespace::Ping_rate_msec);
    }
    Q_PROPERTY(int msec_per_ping READ msec_per_ping WRITE set_msec_per_ping NOTIFY msecPerPingUpdate)

    /**
     * @brief Get the speed of sound (mm/s) used for calculating the distance from time-of-flight
     *
     * @return uint32_t
     */
    uint32_t speed_of_sound() { return _speed_of_sound; }

    /**
     * @brief Set speed of sound (mm/s) used for calculating distance from time-of-flight
     *
     * @param speed_of_sound
     */
    void set_speed_of_sound(uint32_t speed_of_sound)
    {
        ping_msg_ping1D_set_speed_of_sound m;
        m.set_speed(speed_of_sound);
        m.updateChecksum();
        writeMessage(m);
        request(Ping1DNamespace::Speed_of_sound);
    }
    Q_PROPERTY(int speed_of_sound READ speed_of_sound WRITE set_speed_of_sound NOTIFY speedOfSoundUpdate)

    /**
     * @brief Get the processor temperature (centi-degrees C)
     *
     * @return uint16_t
     */
    uint16_t processor_temperature() { return _processor_temperature; }
    Q_PROPERTY(int processor_temperature READ processor_temperature NOTIFY processorTemperatureUpdate)

    /**
     * @brief Get the pcb temperature (centi-degrees C)
     *
     * @return uint16_t
     */
    uint16_t pcb_temperature() { return _pcb_temperature; }
    Q_PROPERTY(int pcb_temperature READ pcb_temperature NOTIFY pcbTemperatureUpdate)

    /**
     * @brief Get the board voltage (milliVolts)
     *
     * @return uint16_t
     */
    uint16_t board_voltage() { return _board_voltage; }
    Q_PROPERTY(int board_voltage READ board_voltage NOTIFY boardVoltageUpdate)

    /**
     * @brief Return poll frequency
     *
     * @return float
     */
    float pollFrequency();

    /**
     * @brief Set poll frequency
     *
     * @param pollFrequency
     */
    void setPollFrequency(float pollFrequency);
    Q_PROPERTY(float pollFrequency READ pollFrequency WRITE setPollFrequency NOTIFY pollFrequencyUpdate)

    /**
     * @brief Return last ascii_text message
     *
     * @return QString
     */
    QString asciiText() { return _ascii_text; }
    Q_PROPERTY(QString ascii_text READ asciiText NOTIFY asciiTextUpdate)

    /**
     * @brief Return last err_msg message
     *
     * @return QString
     */
    QString errMsg() { return _err_msg; }
    Q_PROPERTY(QString err_msg READ errMsg NOTIFY errMsgUpdate)

    // TODO, maybe store history/filtered history of values in this
    // object for access by different visual elements without need to recompute
    // TODO install filters here?

    /**
     * @brief Request message id
     *
     * @param id
     */
    Q_INVOKABLE void request(int id);

    /**
     * @brief Do firmware sensor update
     *
     * @param fileUrl firmware file path
     * @param sendPingGotoBootloader Use "goto bootloader" message
     */
    Q_INVOKABLE void firmwareUpdate(QString fileUrl, bool sendPingGotoBootloader = true);

signals:
    /**
     * @brief emitted when propriety changes
     */
///@{
    void asciiTextUpdate();
    void errMsgUpdate();

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
    void speedOfSoundUpdate();

    void processorTemperatureUpdate();
    void pcbTemperatureUpdate();
    void boardVoltageUpdate();
///@}

    /**
     * @brief Emit firmware update progress
     *
     * @param progress
     */
    void flashProgress(float progress);

    /**
     * @brief Emit when firmware update finished
     *
     */
    void flashComplete();

    /**
     * @brief Emit when poll frequency changes
     *
     */
    void pollFrequencyUpdate();
private:
    /**
     * @brief Sensor variables
     */
///@{
    QString _ascii_text;
    QString _err_msg;

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
    uint32_t _speed_of_sound;

    uint16_t _processor_temperature;
    uint16_t _pcb_temperature;
    uint16_t _board_voltage;
///@}

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
    QTimer _periodicRequestTimer; // For automatic periodic updates (board voltage and temperature)

    QSharedPointer<QProcess> _firmwareProcess;
};
