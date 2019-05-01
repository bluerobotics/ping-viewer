#pragma once

#include <functional>

#include <QProcess>
#include <QSharedPointer>
#include <QTimer>

#include "parsers/parser.h"
#include "parsers/parser_ping.h"
#include "pingmessage/ping_ping1d.h"
#include "protocoldetector.h"
#include "sensor.h"

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
    ~Ping();

    /**
     * @brief Add new connection
     *
     * @param connType connection type
     * @param connString arguments for the new connection
     */
    Q_INVOKABLE void connectLink(AbstractLinkNamespace::LinkType connType, const QStringList& connString);

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
     * @brief Return if sensor is enabled
     *
     * @return true if enabled
     * @return false if not enabled
     */
    bool pingEnable() { return _ping_enable; };

    /**
     * @brief Enable or disable the sensor
     *
     * @param enabled
     */
    void pingEnable(bool enabled)
    {
        ping_ping1D_set_ping_enable m;
        m.set_ping_enabled(enabled);
        m.updateChecksum();
        writeMessage(m);
        request(PingPing1DNamespace::PingEnable);
    }

    Q_PROPERTY(bool pingEnable READ pingEnable WRITE pingEnable NOTIFY pingEnableUpdate)

    /**
     * @brief Return firmware major version
     *
     * @return uint16_t firmware major version number
     */
    uint16_t firmware_version_major() { return _firmware_version_major; }
    Q_PROPERTY(int firmware_version_major READ firmware_version_major NOTIFY firmwareVersionMajorUpdate)

    /**
     * @brief Return firmware minor version
     *
     * @return uint16_t firmware minor version number
     */
    uint16_t firmware_version_minor() { return _firmware_version_minor; }
    Q_PROPERTY(int firmware_version_minor READ firmware_version_minor NOTIFY firmwareVersionMinorUpdate)

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
     * @brief Return pulse emission in ms
     *
     * @return uint16_t
     */
    uint16_t pulse_duration() { return _pulse_duration; }
    Q_PROPERTY(int pulse_duration READ pulse_duration NOTIFY pulseDurationUpdate)

    /**
     * @brief Return start distance of sonar points in mm
     *
     * @return uint32_t
     */
    uint32_t start_mm() { return _scan_start; }

    /**
     * @brief Set sensor start analyze distance in mm
     *
     * @param start_mm
     */
    void set_start_mm(int start_mm)
    {
        ping_ping1D_set_range m;
        m.set_scan_start(start_mm);
        m.set_scan_length(_scan_length);
        m.updateChecksum();
        writeMessage(m);
        request(PingPing1DNamespace::Range);
    }
    Q_PROPERTY(int start_mm READ start_mm WRITE set_start_mm NOTIFY scanStartUpdate)

    /**
     * @brief return points length in mm
     *
     * @return uint32_t
     */
    uint32_t length_mm() { return _scan_length; }

    /**
     * @brief Set sensor window analysis size
     *
     * @param length_mm
     */
    void set_length_mm(int length_mm)
    {
        ping_ping1D_set_range m;
        m.set_scan_start(_scan_start);
        m.set_scan_length(length_mm);
        m.updateChecksum();
        writeMessage(m);
        request(PingPing1DNamespace::Range);
    }
    Q_PROPERTY(int length_mm READ length_mm WRITE set_length_mm NOTIFY scanLengthUpdate)

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
        ping_ping1D_set_gain_index m;
        m.set_gain_index(gain_index);
        m.updateChecksum();
        writeMessage(m);
        request(PingPing1DNamespace::GainIndex);
    }
    Q_PROPERTY(int gain_index READ gain_index WRITE set_gain_index NOTIFY gainIndexUpdate)

    /**
     * @brief Return last array of points
     *
     * @return QVector<double>
     */
    QVector<double> points() { return _points; }
    Q_PROPERTY(QVector<double> points READ points NOTIFY pointsUpdate)

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
        ping_ping1D_set_mode_auto m;
        m.set_mode_auto(mode_auto);
        m.updateChecksum();
        writeMessage(m);
        request(PingPing1DNamespace::ModeAuto);
    }
    Q_PROPERTY(bool mode_auto READ mode_auto WRITE set_mode_auto NOTIFY modeAutoUpdate)

    /**
     * @brief Do continuous start
     *
     * @param id
     */
    void do_continuous_start(PingPing1DNamespace::ping_ping1D_id id)
    {
        ping_ping1D_continuous_start m;
        m.set_id(static_cast<int>(id));
        m.updateChecksum();
        writeMessage(m);
    }

    /**
     * @brief Do continuous stop
     *
     * @param id
     */
    void do_continuous_stop(PingPing1DNamespace::ping_ping1D_id id)
    {
        ping_ping1D_continuous_stop m;
        m.set_id(static_cast<int>(id));
        m.updateChecksum();
        writeMessage(m);
    }

    /**
     * @brief Get time between pings in ms
     *
     * @return uint16_t
     */
    uint16_t ping_interval() { return _ping_interval; }

    /**
     * @brief Set time between pings in ms
     *
     * @param ping_interval
     */
    void set_ping_interval(uint16_t ping_interval)
    {
        ping_ping1D_set_ping_interval m;
        m.set_ping_interval(ping_interval);
        m.updateChecksum();
        writeMessage(m);
        request(PingPing1DNamespace::PingInterval);
    }
    Q_PROPERTY(int ping_interval READ ping_interval WRITE set_ping_interval NOTIFY pingIntervalUpdate)

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
        ping_ping1D_set_speed_of_sound m;
        m.set_speed_of_sound(speed_of_sound);
        m.updateChecksum();
        writeMessage(m);
        request(PingPing1DNamespace::SpeedOfSound);
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
     * @brief Return ping frequency
     *
     * @return float
     */
    float pingFrequency() { return _ping_interval ? static_cast<int>(1000/_ping_interval) : 0; };

    /**
     * @brief Set ping frequency
     *
     * @param pingFrequency
     */
    void setPingFrequency(float pingFrequency);
    Q_PROPERTY(float pingFrequency READ pingFrequency WRITE setPingFrequency NOTIFY pingIntervalUpdate)

    /**
     * @brief Return the max frequency that the sensor can work
     *
     * @return int
     */
    int pingMaxFrequency() { return _pingMaxFrequency; }
    Q_PROPERTY(int pingMaxFrequency READ pingMaxFrequency CONSTANT)
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
    QString errMsg() { return _nack_msg; }
    Q_PROPERTY(QString err_msg READ errMsg NOTIFY nackMsgUpdate)

    /**
     * @brief Return number of parser errors
     *
     * @return int
     */
    int parserErrors() { return _parser ? _parser->errors : 0; }
    Q_PROPERTY(int parser_errors READ parserErrors NOTIFY parserErrorsUpdate)

    /**
     * @brief Return number of successfully parsed messages
     *
     * @return int
     */
    int parsedMsgs() { return _parser ? _parser->parsed : 0; }
    Q_PROPERTY(int parsed_msgs READ parsedMsgs NOTIFY parsedMsgsUpdate)
    // TODO: maybe store history/filtered history of values in this
    // object for access by different visual elements without need to recompute
    // TODO: install filters here?

    /**
     * @brief Return the number of messages that we requested and did not received
     *
     * @return int
     */
    int lostMessages() { return _lostMessages; }
    Q_PROPERTY(int lost_messages READ lostMessages NOTIFY lostMessagesUpdate)

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
     * @param baud baud rate value
     * @param verify this variable is true when all
     */
    Q_INVOKABLE void firmwareUpdate(QString fileUrl, bool sendPingGotoBootloader = true, int baud = 57600,
                                    bool verify = true);

signals:
    /**
     * @brief emitted when propriety changes
     */
///@{
    void asciiTextUpdate();
    void nackMsgUpdate();

    void srcIdUpdate();
    void dstIdUpdate();
    void deviceTypeUpdate();
    void deviceModelUpdate();
    void firmwareVersionMajorUpdate();
    void firmwareVersionMinorUpdate();

    void distanceUpdate();
    void pingNumberUpdate();
    void confidenceUpdate();
    void pulseDurationUpdate();
    void scanStartUpdate();
    void scanLengthUpdate();
    void gainIndexUpdate();
    void pointsUpdate();

    void modeAutoUpdate();
    void pingIntervalUpdate();
    void speedOfSoundUpdate();

    void processorTemperatureUpdate();
    void pcbTemperatureUpdate();
    void boardVoltageUpdate();

    void pingEnableUpdate();

    void parserErrorsUpdate();
    void parsedMsgsUpdate();
///@}

    /**
    * @brief Emit when lost messages is updated
    *
    */
    void lostMessagesUpdate();

private:
    Q_DISABLE_COPY(Ping)
    /**
     * @brief Sensor variables
     */
///@{
    QString _ascii_text = QString();
    QString _nack_msg = QString();

    uint8_t _srcId = 0;
    uint8_t _dstId = 0;

    uint8_t _device_type = 0;
    uint8_t _device_model = 0;
    uint16_t _firmware_version_major = 0;
    uint16_t _firmware_version_minor = 0;

    uint32_t _distance = 0; // mm
    uint16_t _confidence = 0; // 0-100%
    uint16_t _pulse_duration = 0;
    uint32_t _ping_number = 0;
    uint32_t _scan_start = 0;
    uint32_t _scan_length = 0;
    uint32_t _gain_index = 0;
    uint32_t _speed_of_sound = 0;

    uint16_t _processor_temperature = 0;
    uint16_t _pcb_temperature = 0;
    uint16_t _board_voltage = 0;

    bool _ping_enable = false;
///@}

    static const uint16_t _num_points = 200;

    // TODO: maybe use vector or uint8_t[] here
    // QVector is only required if points need to be exposed to qml
    //QVector<int> _points;
    QVector<double> _points;

    bool _mode_auto = 0;
    uint16_t _ping_interval = 0;
    static const int _pingMaxFrequency;

    // TODO: const &
    void handleMessage(ping_message msg); // handle incoming message
    void writeMessage(const ping_message& msg); // write a message to link

    void loadLastPingConfigurationSettings();
    void updatePingConfigurationSettings();
    void setLastPingConfiguration();

    /**
     * @brief Internal function used to use as a flash callback
     *
     * @param fileUrl Firmware file path
     * @param sendPingGotoBootloader Use "goto bootloader" message
     * @param baud baud rate value
     * @param verify this variable is true when all
     */
    void flash(const QString& fileUrl, bool sendPingGotoBootloader, int baud, bool verify);

    /**
     * @brief Reset sensor local variables
     *  TODO: This variables should be moved to a structure
     *
     */
    void resetSensorLocalVariables();

    /**
     * @brief This saves the last configuration ID
     *  This value need to be set as an invalid one (-1)
     *  To allow sensor reconfiguration.
     *
     *  This should be removed after creating a PingSensor class that does not deal with connections and
     *  Firmware update.
     *  Check: https://github.com/bluerobotics/ping-viewer/issues/406
     */
    int _lastPingConfigurationSrcId = -1;

    /**
     * @brief Start the pre configuration process of the sensor
     *
     */
    void startPreConfigurationProcess();

    /**
     * @brief Take care of github payload and detect new versions available
     *
     * @param jsonDocument
     */
    void checkNewFirmwareInGitHubPayload(const QJsonDocument& jsonDocument);

    // For automatic periodic updates (board voltage and temperature)
    QTimer _periodicRequestTimer;

    QSharedPointer<QProcess> _firmwareProcess;

    struct settingsConfiguration {
        /*
        Data need to be long long int:
            - Otherwise an error can happen if we are using uint32 with int 32 arch
            - If we send the value to QVariant, that does not know what to do with long int
        */
        long long int defaultValue = 0;
        long long int minValue = INT_MIN;
        long long int maxValue = INT_MAX;
        std::function<long long int(void)> getClassValue;
        std::function<void(long long int)> setClassValue;
        long long int value = 0;
        void set(QVariant variantValue)
        {
            int testValue = variantValue.toInt();
            value = testValue < minValue || testValue > maxValue ?
                    defaultValue : testValue;
        }
    };

    /**
     * @brief Map to do automatic configuration of the sensor
     *
     * The X_ prefix is to identify the order that this keys should be used.
     * This is done by using method keys(), that will sort it.
     * The only way to make it around of such approach is to use:
     * QVector<QPair<QString, settingsConfiguration>>,
     * but this will make further code boilerplate because of QPair access
     *
     */
    QMap<QString, settingsConfiguration> _pingConfiguration {
        {   {"1_pingInterval"}, {
                66, 20, 1000,
                std::bind(&Ping::ping_interval, this),
                [this](long long int value) {set_ping_interval(value);}
            }
        },
        {   {"1_speedOfSound"}, {
                1500000, 50000, 10000000,
                std::bind(&Ping::speed_of_sound, this),
                [this](long long int value) {set_speed_of_sound(value);}
            }
        },
        {   {"2_automaticMode"}, {
                true, false, true,
                std::bind(&Ping::mode_auto, this),
                [this](long long int value) {set_mode_auto(value);}
            }
        },
        {   {"3_gainIndex"}, {
                0, 0, 6,
                std::bind(&Ping::gain_index, this),
                [this](long long int value) {set_gain_index(value);}
            }
        },
        {   {"3_lengthDistance"}, {
                5000, 500, 70000,
                std::bind(&Ping::length_mm, this),
                [this](long long int value) {set_length_mm(value);}
            }
        },
        {   {"3_startDistance"}, {
                0, 0, 70000,
                std::bind(&Ping::start_mm, this),
                [this](long long int value) {set_start_mm(value);}
            }
        },
    };

    // total of lost messages
    int _lostMessages = 0;

    struct messageStatus {
        // Requested and acknowledge
        int ack = 0;
        // Requested and not acknowledge
        int nack = 0;
        // Number of waiting replies
        int waiting = 0;
    };
    friend QDebug operator<<(QDebug d, const Ping::messageStatus& other);
    QHash<PingPing1DNamespace::ping_ping1D_id, messageStatus> requestedIds;
};
