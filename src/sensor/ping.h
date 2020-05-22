#pragma once

#include <functional>

#include <QProcess>
#include <QSharedPointer>
#include <QTimer>

#include "pingsensor.h"
#include "protocoldetector.h"
#include <ping-message-common.h>
#include <ping-message-ping1d.h>
#include <ping-message.h>

/**
 * @brief Define ping sensor
 * 1D Sonar
 *
 */
class Ping : public PingSensor {
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
    Q_INVOKABLE void connectLink(
        AbstractLinkNamespace::LinkType connType, const QStringList& connString) final override;

    /**
     * @brief Reset device settings
     *
     */
    Q_INVOKABLE void resetSettings() override final;

    /**
     * @brief debug function
     */
    void printSensorInformation() const override final;

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
        ping1d_set_ping_enable m;
        m.set_ping_enabled(enabled);
        m.updateChecksum();
        writeMessage(m);
        request(Ping1dId::PING_ENABLE);
    }

    Q_PROPERTY(bool pingEnable READ pingEnable WRITE pingEnable NOTIFY pingEnableChanged)

    /**
     * @brief Return distance in mm
     *
     * @return uint32_t
     */
    uint32_t distance() { return _distance; }
    Q_PROPERTY(int distance READ distance NOTIFY distanceChanged)

    /**
     * @brief Return number of pings emitted
     *
     * @return uint32_t
     */
    uint32_t ping_number() { return _ping_number; }
    Q_PROPERTY(int ping_number READ ping_number NOTIFY pingNumberChanged)

    /**
     * @brief Return depth confidence
     *
     * @return uint8_t
     */
    uint8_t confidence() { return _confidence; }
    Q_PROPERTY(int confidence READ confidence NOTIFY confidenceChanged)

    /**
     * @brief Return pulse emission in ms
     *
     * @return uint16_t
     */
    uint16_t transmit_duration() { return _transmit_duration; }
    Q_PROPERTY(int transmit_duration READ transmit_duration NOTIFY transmitDurationChanged)

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
        ping1d_set_range m;
        m.set_scan_start(start_mm);
        m.set_scan_length(_scan_length);
        m.updateChecksum();
        writeMessage(m);
        request(Ping1dId::RANGE);
    }
    Q_PROPERTY(int start_mm READ start_mm WRITE set_start_mm NOTIFY scanStartChanged)

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
        ping1d_set_range m;
        m.set_scan_start(_scan_start);
        m.set_scan_length(length_mm);
        m.updateChecksum();
        writeMessage(m);
        request(Ping1dId::RANGE);
    }
    Q_PROPERTY(int length_mm READ length_mm WRITE set_length_mm NOTIFY scanLengthChanged)

    /**
     * @brief Return gain setting
     *
     * @return uint32_t
     */
    uint32_t gain_setting() { return _gain_setting; }

    /**
     * @brief Set sensor gain index
     *
     * @param gain_setting
     */
    void set_gain_setting(int gain_setting)
    {
        ping1d_set_gain_setting m;
        m.set_gain_setting(gain_setting);
        m.updateChecksum();
        writeMessage(m);
        request(Ping1dId::GAIN_SETTING);
    }
    Q_PROPERTY(int gain_setting READ gain_setting WRITE set_gain_setting NOTIFY gainSettingChanged)

    /**
     * @brief Return last array of points
     *
     * @return QVector<double>
     */
    QVector<double> points() { return _points; }
    Q_PROPERTY(QVector<double> points READ points NOTIFY pointsChanged)

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
        ping1d_set_mode_auto m;
        m.set_mode_auto(mode_auto);
        m.updateChecksum();
        writeMessage(m);
        request(Ping1dId::MODE_AUTO);
    }
    Q_PROPERTY(bool mode_auto READ mode_auto WRITE set_mode_auto NOTIFY modeAutoChanged)

    /**
     * @brief These are ids that the sensor can continuously output.
     * The sensor only supports continuous profile output
     */
    enum ContinuousId : uint16_t { PROFILE = Ping1dId::PROFILE };

    /**
     * @brief Do continuous start
     *
     * @param id
     */
    void do_continuous_start(ContinuousId id)
    {
        ping1d_continuous_start m;
        m.set_id(id);
        m.updateChecksum();
        writeMessage(m);
    }

    /**
     * @brief Do continuous stop
     *
     * @param id
     */
    void do_continuous_stop(ContinuousId id)
    {
        ping1d_continuous_stop m;
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
        ping1d_set_ping_interval m;
        m.set_ping_interval(ping_interval);
        m.updateChecksum();
        writeMessage(m);
        request(Ping1dId::PING_INTERVAL);
    }
    Q_PROPERTY(int ping_interval READ ping_interval WRITE set_ping_interval NOTIFY pingIntervalChanged)

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
        ping1d_set_speed_of_sound m;
        m.set_speed_of_sound(speed_of_sound);
        m.updateChecksum();
        writeMessage(m);
        request(Ping1dId::SPEED_OF_SOUND);
    }
    Q_PROPERTY(int speed_of_sound READ speed_of_sound WRITE set_speed_of_sound NOTIFY speedOfSoundChanged)

    /**
     * @brief Get the processor temperature (centi-degrees C)
     *
     * @return uint16_t
     */
    uint16_t processor_temperature() const { return _processor_temperature; }
    Q_PROPERTY(int processor_temperature READ processor_temperature NOTIFY processorTemperatureChanged)

    /**
     * @brief Get the pcb temperature (centi-degrees C)
     *
     * @return uint16_t
     */
    uint16_t pcb_temperature() const { return _pcb_temperature; }
    Q_PROPERTY(int pcb_temperature READ pcb_temperature NOTIFY pcbTemperatureChanged)

    /**
     * @brief Get the board voltage (milliVolts)
     *
     * @return uint16_t
     */
    uint16_t board_voltage() const { return _board_voltage; }
    Q_PROPERTY(int board_voltage READ board_voltage NOTIFY boardVoltageChanged)

    /**
     * @brief Return ping frequency
     *
     * @return float
     */
    float pingFrequency() { return _ping_interval ? static_cast<int>(1000 / _ping_interval) : 0; };

    /**
     * @brief Set ping frequency
     *
     * @param pingFrequency
     */
    void setPingFrequency(float pingFrequency);
    Q_PROPERTY(float pingFrequency READ pingFrequency WRITE setPingFrequency NOTIFY pingIntervalChanged)

    /**
     * @brief Return the max frequency that the sensor can work
     *
     * @return int
     */
    int pingMaxFrequency() { return _pingMaxFrequency; }
    Q_PROPERTY(int pingMaxFrequency READ pingMaxFrequency CONSTANT)

    /**
     * @brief Do firmware sensor update
     *
     * @param fileUrl firmware file path
     * @param sendPingGotoBootloader Use "goto bootloader" message
     * @param baud baud rate value
     * @param verify this variable is true when all
     */
    Q_INVOKABLE void firmwareUpdate(
        QString fileUrl, bool sendPingGotoBootloader = true, int baud = 57600, bool verify = true) final override;

    /**
     * @brief request a profile message from the device
     */
    Q_INVOKABLE void emitPing() const { request(Ping1dId::PROFILE); }

signals:
    /**
     * @brief emitted when property changes
     */
    ///@{
    void distanceChanged();
    void pingNumberChanged();
    void confidenceChanged();
    void transmitDurationChanged();
    void scanStartChanged();
    void scanLengthChanged();
    void gainSettingChanged();
    void pointsChanged();

    void modeAutoChanged();
    void pingEnableChanged();
    void pingIntervalChanged();
    void speedOfSoundChanged();

    void boardVoltageChanged();
    void pcbTemperatureChanged();
    void processorTemperatureChanged();
    ///@}

private:
    Q_DISABLE_COPY(Ping)
    /**
     * @brief Sensor variables
     */
    ///@{
    // The firmware defaults at boot
    static const bool _firmwareDefaultAutoMode;
    static const int _firmwareDefaultGainSetting;
    static const bool _firmwareDefaultPingEnable;
    static const uint16_t _firmwareDefaultPingInterval;
    static const uint32_t _firmwareDefaultSpeedOfSound;

    uint16_t _board_voltage;
    uint8_t _device_model;
    uint32_t _distance = 0; // mm
    uint16_t _confidence = 0; // 0-100%
    uint16_t _transmit_duration = 0;
    uint32_t _ping_number = 0;
    uint32_t _scan_start = 0;
    uint32_t _scan_length = 0;
    uint32_t _gain_setting = 0;
    uint32_t _speed_of_sound = 0;
    bool _ping_enable;
    uint16_t _pcb_temperature;
    uint16_t _processor_temperature;
    ///@}

    static const uint16_t _num_points = 200;

    /**
     * @brief The points received by the sensor
     *  Such points are shared between the sensor and the interface in a normalized format [1-0]
     *  Where the maximum value is the max power and 0 the lowest power
     *  QVector<double> is used to share such points with the QML interface and the viewer widgets
     *
     */
    QVector<double> _points;

    bool _mode_auto = 0;
    uint16_t _ping_interval = 0;
    static const int _pingMaxFrequency;

    void handleMessage(const ping_message& msg) final; // handle incoming message

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
            value = testValue < minValue || testValue > maxValue ? defaultValue : testValue;
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
        {{"1_pingInterval"},
            {66, 20, 1000, std::bind(&Ping::ping_interval, this),
                [this](long long int value) { set_ping_interval(value); }}},
        {{"1_speedOfSound"},
            {1500000, 50000, 10000000, std::bind(&Ping::speed_of_sound, this),
                [this](long long int value) { set_speed_of_sound(value); }}},
        {{"2_automaticMode"},
            {true, false, true, std::bind(&Ping::mode_auto, this),
                [this](long long int value) { set_mode_auto(value); }}},
        {{"3_gainIndex"},
            {0, 0, 6, std::bind(&Ping::gain_setting, this), [this](long long int value) { set_gain_setting(value); }}},
        {{"3_lengthDistance"},
            {5000, 500, 120000, std::bind(&Ping::length_mm, this),
                [this](long long int value) { set_length_mm(value); }}},
        {{"3_startDistance"},
            {0, 0, 120000, std::bind(&Ping::start_mm, this), [this](long long int value) { set_start_mm(value); }}},
    };

    struct messageStatus {
        // Requested and acknowledge
        int ack = 0;
        // Requested and not acknowledge
        int nack = 0;
        // Number of waiting replies
        int waiting = 0;
    };
    friend QDebug operator<<(QDebug d, const Ping::messageStatus& other);
    QHash<uint16_t, messageStatus> requestedIds;
};
