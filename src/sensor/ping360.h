#pragma once

#include <cmath>
#include <functional>

#include <QProcess>
#include <QSharedPointer>
#include <QTimer>

#include "parser.h"
#include "parser-ping.h"
#include "ping-message-common.h"
#include "ping-message-ping360.h"
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
    Q_INVOKABLE void connectLink(AbstractLinkNamespace::LinkType connType, const QStringList& connString) final override;

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
        // Force delta to be positive and inside our polar space
        while(delta < 0) {
            delta += _angularResolutionGrad;
        }

        int nextPoint = (_angle+delta)%_angularResolutionGrad;

        ping360_transducer msg;
        msg.set_mode(1);
        msg.set_gain_setting(_gain_setting);
        msg.set_angle(nextPoint);
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
        emit transmitDurationChanged();
    }

    /**
     * @brief Return pulse emission in ms
     *
     * @return uint16_t
     */
    uint16_t transmit_duration() { return _transmit_duration; }
    Q_PROPERTY(int transmit_duration READ transmit_duration WRITE set_transmit_duration NOTIFY transmitDurationChanged)

    /**
     * @brief Return the sample period in ms
     *
     * @return int
     */
    int sample_period()
    {
        return _sample_period;
    }
    Q_PROPERTY(int sample_period READ sample_period NOTIFY samplePeriodChanged)

    /**
     * @brief Set the transmit frequency in Hz
     *
     * @param transmit_frequency
     */
    void set_transmit_frequency(int transmit_frequency)
    {
        _transmit_frequency = transmit_frequency;
        emit transmitFrequencyChanged();
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
    uint16_t angle() { return (_angle + _angle_offset)%_angularResolutionGrad; }
    Q_PROPERTY(int angle READ angle NOTIFY angleChanged)

    /**
     * @brief The sonar communicates the _sample_period in units of 25nsec ticks
     * @return inter-sample period in nanoseconds
     */
    double samplePeriod() { return _sample_period * _samplePeriodTickDuration; }

    /**
     * @brief return points length in mm
     *
     * @return uint32_t
     */
    double range() { return samplePeriod() * _num_points * _speed_of_sound / 2; }

    /**
     * @brief Set sensor window analysis size
     *
     * @param newRange
     */
    void set_range(uint newRange)
    {
        if(newRange != range()) {
            _sample_period = ceil(2*newRange/(_num_points*_speed_of_sound*_samplePeriodTickDuration));
            emit rangeChanged();
        }
    }
    Q_PROPERTY(double range READ range WRITE set_range NOTIFY rangeChanged)

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
        emit gainSettingChanged();
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
        if (speed_of_sound != _speed_of_sound) {
            // range depends on _speed_of_sound
            // we adjust _speed_of_sound, without affecting the current range setting
            double desiredRange = round(range());
            _speed_of_sound = speed_of_sound;
            _sample_period = 2*desiredRange/(_num_points*_speed_of_sound*_samplePeriodTickDuration);
            emit speedOfSoundChanged();
            emit rangeChanged();
        }
    }
    Q_PROPERTY(int speed_of_sound READ speed_of_sound WRITE set_speed_of_sound NOTIFY speedOfSoundChanged)

    /**
     * @brief Returns the angle offset from sample position
     *  TODO: Maybe this should be in the viewer configuration and not in the sensor class
     * @return int
     */
    int angle_offset() { return _angle_offset; }

    /**
     * @brief Set angle offset from sample position
     *
     * @param angle_offset
     */
    void set_angle_offset(int angle_offset)
    {
        if(angle_offset != _angle_offset) {
            _angle_offset = angle_offset;
            emit angleOffsetChanged();
        }
    }
    Q_PROPERTY(int angle_offset READ angle_offset WRITE set_angle_offset NOTIFY angleOffsetChanged)

    /**
     * @brief Angular sample speed
     *
     * @return uint
     */
    int angular_speed() { return _angular_speed; }

    /**
     * @brief Set the angular speed
     *
     * @param angular_speed
     */
    void set_angular_speed(int angular_speed)
    {
        if(angular_speed != _angular_speed) {
            _angular_speed = angular_speed;
            emit angularSpeedChanged();
        }
    }
    Q_PROPERTY(int angular_speed READ angular_speed WRITE set_angular_speed NOTIFY angularSpeedChanged)

    /**
     * @brief Reverse sonar direction
     *
     * @return bool
     */
    bool reverse_direction() { return _reverse_direction; }

    /**
     * @brief Set reverse direction
     *
     * @param reverse_direction
     */
    void set_reverse_direction(bool reverse_direction)
    {
        if(reverse_direction != _reverse_direction) {
            _reverse_direction = reverse_direction;
            emit reverseDirectionChanged();
        }
    }
    Q_PROPERTY(bool reverse_direction READ reverse_direction WRITE set_reverse_direction NOTIFY reverseDirectionChanged)

    /**
     * @brief Return the number of points
     *
     * @return int
     */
    int number_of_points() { return _num_points; }
    Q_PROPERTY(int number_of_points READ number_of_points NOTIFY numberOfPointsChanged)

    /**
     * @brief Return sector size in degrees
     *
     * @return int
     */
    int sectorSize() { return round(_sectorSize * 360/400.0); }

    /**
     * @brief Set sector size in degrees
     *
     * @param sectorSize
     */
    void setSectorSize(int sectorSize)
    {
        int sectorSizeGrad = round(sectorSize * 400/360.0);

        if(_sectorSize != sectorSizeGrad) {
            _sectorSize = sectorSizeGrad;
            emit sectorSizeChanged();
        }
    }

    Q_PROPERTY(int sectorSize READ sectorSize WRITE setSectorSize NOTIFY sectorSizeChanged)

    /**
     * @brief Do firmware sensor update
     *
     * @param fileUrl firmware file path
     * @param sendPingGotoBootloader Use "goto bootloader" message
     * @param baud baud rate value
     * @param verify this variable is true when all
     */
    Q_INVOKABLE void firmwareUpdate(QString fileUrl, bool sendPingGotoBootloader = true, int baud = 57600,
                                    bool verify = true) final override;

signals:
    /**
     * @brief emitted when property changes
     */
///@{
    void angleChanged();
    void angleOffsetChanged();
    void angularSpeedChanged();
    void dataChanged();
    void gainSettingChanged();
    void numberOfPointsChanged();
    void pingNumberChanged();
    void reverseDirectionChanged();
    void samplePeriodChanged();
    void sectorSizeChanged();
    void rangeChanged();
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

    // firmware constants
    static const uint16_t _firmwareMaxNumberOfPoints = 1200;
    // The firmware defaults at boot
    static const uint8_t _firmwareDefaultGainSetting = 0;
    static const uint16_t _firmwareDefaultAngle = 0;
    static const uint16_t _firmwareDefaultTransmitDuration = 32;
    static const uint16_t _firmwareDefaultSamplePeriod = 80;
    static const uint16_t _firmwareDefaultTransmitFrequency = 740;
    static const uint16_t _firmwareDefaultNumberOfSamples = 1024;

    // The default transmit frequency to operate with
    static const uint16_t _viewerDefaultTransmitFrequency = 750;
    static const uint16_t _viewerDefaultNumberOfSamples = _firmwareMaxNumberOfPoints;

    uint16_t _angle = _firmwareDefaultAngle;
    uint16_t _transmit_duration = _firmwareDefaultTransmitDuration;
    uint32_t _gain_setting = _firmwareDefaultGainSetting;
    uint16_t _num_points = _viewerDefaultNumberOfSamples;
    uint16_t _sample_period = _firmwareDefaultSamplePeriod;
    uint16_t _transmit_frequency = _viewerDefaultTransmitFrequency;
    QVector<double> _data;
///@}

    // _sample_period is the number of timer ticks between each data point
    // each timer tick has a duration of 25 nanoseconds
    static constexpr double _samplePeriodTickDuration = 25e-9;
    uint32_t _ping_number = 0;
    // Ping360 has a 200 offset by default
    int _angle_offset = 200;
    int _angular_speed = 1;
    uint _central_angle = 1;
    bool _reverse_direction = false;
    uint32_t _speed_of_sound = 1500;

    int _angularResolutionGrad = 400;
    // The motor takes 4s to run a full circle
    float _motorSpeedGradMs = 4000/_angularResolutionGrad;
    // Right now the max value is 1200 for ping360
    // We are saving 2k of the memory for future proof modifications
    int _maxNumberOfPoints = 2048;
    // The sensor can take 4s to answer, we are also using an extra 200ms for latency
    int _sensorTimeout = 4200;

    // Sector size in gradians, default is full circle
    int _sectorSize = 400;

    QTimer _timeoutProfileMessage;

    void handleMessage(const ping_message& msg) final; // handle incoming message

    void loadLastSensorConfigurationSettings();
    void updateSensorConfigurationSettings();
    void setLastSensorConfiguration();

    /**
     * @brief Request the next profile based in the class configuration
     *
     */
    void requestNextProfile();

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
