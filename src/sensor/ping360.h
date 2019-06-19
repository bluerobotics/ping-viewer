#pragma once

#include <functional>

#include <QProcess>
#include <QSharedPointer>
#include <QTimer>

#include "parsers/parser.h"
#include "parsers/parser_ping.h"
#include "pingmessage/ping_ping360.h"
#include "protocoldetector.h"
#include "pingsensor.h"

/**
 * @brief Define Ping360 sensor
 * Ping 360 Sonar
 *
 */
class Ping360 : public PingSensor
{
    Q_OBJECT
public:

    Ping360();
    ~Ping360();

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
    void printSensorInformation() const override final;

    /**
     * @brief Helper function to request profiles from sensor based in the actual position
     *
     * @param delta number of grads/steps from the actual position
     */
    Q_INVOKABLE void deltaStep(int delta)
    {
        ping360_transducer msg;
        msg.set_mode(1);
        msg.set_gain_setting(_gain_setting);
        msg.set_angle((_angle+delta)%400);
        msg.set_transmit_duration(_transmit_duration);
        msg.set_sample_period(_sample_period);
        msg.set_transmit_frequency(_transmit_frequency);
        msg.set_number_of_samples(_num_points);
        msg.set_transmit(1);

        msg.updateChecksum();
        writeMessage(msg);
    }

    /**
     * @brief Return number of pings emitted
     *
     * @return uint32_t
     */
    uint32_t ping_number() { return _ping_number; }
    Q_PROPERTY(int ping_number READ ping_number NOTIFY pingNumberChanged)

    /**
     * @brief Set pulse emission in ms
     *
     * @param transmit_duration
     */
    void set_transmit_duration(int transmit_duration)
    {
        _transmit_duration = transmit_duration;
    }

    /**
     * @brief Return pulse emission in ms
     *
     * @return uint16_t
     */
    uint16_t transmit_duration() { return _transmit_duration; }
    Q_PROPERTY(int transmit_duration READ transmit_duration WRITE set_transmit_duration NOTIFY transmitDurationChanged)

    /**
     * @brief Set the sample period in ms
     *
     * @param sample_period
     */
    void set_sample_period(int sample_period)
    {
        _sample_period = sample_period;
    }

    /**
     * @brief Return the sample period in ms
     *
     * @return int
     */
    int sample_period()
    {
        return _sample_period;
    }
    Q_PROPERTY(int sample_period READ sample_period WRITE set_sample_period NOTIFY samplePeriodChanged)

    /**
     * @brief Set the transmit frequency in Hz
     *
     * @param transmit_frequency
     */
    void set_transmit_frequency(int transmit_frequency)
    {
        _transmit_frequency = transmit_frequency;
    }

    /**
     * @brief Return the transmit frequency in Hz
     *
     * @return int
     */
    int transmit_frequency()
    {
        return _transmit_frequency;
    }
    Q_PROPERTY(int transmit_frequency READ transmit_frequency WRITE set_transmit_frequency NOTIFY transmitFrequencyChanged)

    /**
     * @brief Angle of sensor head in gradians (400)
     *
     * @return uint16_t
     */
    uint16_t angle() { return _angle; }
    Q_PROPERTY(int angle READ angle NOTIFY angleChanged)

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
        _scan_start = start_mm;
    }

    //TODO: update this signal name and others to Changed
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
        Q_UNUSED(length_mm)
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
        _gain_setting = gain_setting;
    }
    Q_PROPERTY(int gain_setting READ gain_setting WRITE set_gain_setting NOTIFY gainSettingChanged)

    /**
     * @brief Return last array of points
     *
     * @return QVector<double>
     */
    QVector<double> data() { return _data; }
    Q_PROPERTY(QVector<double> data READ data NOTIFY dataChanged)

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
        Q_UNUSED(ping_interval)
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
        Q_UNUSED(speed_of_sound)
    }
    Q_PROPERTY(int speed_of_sound READ speed_of_sound WRITE set_speed_of_sound NOTIFY speedOfSoundChanged)

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

    /**
     * @brief Return a qml component that will take care of the main control panel for the sensor
     *
     * @param parent
     * @return QQuickItem* controlPanel
     */
    Q_INVOKABLE QQuickItem* controlPanel(QObject *parent) override final;

    /**
     * @brief Return the visualization widget
     *
     * @param parent
     * @return QQmlComponent* sensorVisualizer
     */
    Q_INVOKABLE QQmlComponent* sensorVisualizer(QObject *parent) final;

signals:
    /**
     * @brief emitted when propriety changes
     */
///@{
    void angleChanged();
    void dataChanged();
    void gainSettingChanged();
    void pingIntervalChanged();
    void pingNumberChanged();
    void samplePeriodChanged();
    void scanLengthChanged();
    void scanStartChanged();
    void speedOfSoundChanged();
    void transmitDurationChanged();
    void transmitFrequencyChanged();
///@}

private:
    Q_DISABLE_COPY(Ping360)
    /**
     * @brief Sensor variables
     */
///@{
    uint16_t _angle = 0;
    uint16_t _transmit_duration = 42;
    uint32_t _ping_number = 0;
    uint32_t _scan_start = 0;
    uint32_t _scan_length = 0;
    uint32_t _gain_setting = 1;
    uint16_t _sample_period = 1300;
    uint32_t _speed_of_sound = 1500;
    uint16_t _transmit_frequency = 500;
///@}

    uint16_t _num_points = 2000;

    QVector<double> _data;

    uint16_t _ping_interval = 0;

    QSharedPointer<QQuickItem> _controlPanel;

    void handleMessage(const ping_message& msg) final; // handle incoming message

    void loadLastSensorConfigurationSettings();
    void updateSensorConfigurationSettings();
    void setLastSensorConfiguration();

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
};
