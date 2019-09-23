#pragma once

#include <algorithm>
#include <cmath>
#include <functional>

#include <QElapsedTimer>
#include <QProcess>
#include <QSharedPointer>
#include <QTimer>

#include "parser.h"
#include "pingparserext.h"
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
     * @brief Reset device settings
     *
     */
    Q_INVOKABLE void resetSettings() override final;

    /**
     * @brief debug function
     */
    void printSensorInformation() const override final;

    /**
     * @brief Helper function to request profiles from sensor based in the actual position
     *
     * @param delta number of grads/steps from the actual position
     * @param transmit request profile data
     */
    Q_INVOKABLE void deltaStep(int delta, bool transmit = true)
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
        msg.set_transmit(transmit);

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
        if (_transmit_duration != transmit_duration) {
            _transmit_duration = transmit_duration;
            emit transmitDurationChanged();
        }
    }

    /**
     * @brief Return pulse emission in ms
     *
     * @return uint16_t
     */
    uint16_t transmit_duration() { return _transmit_duration; }
    Q_PROPERTY(int transmit_duration READ transmit_duration WRITE set_transmit_duration NOTIFY transmitDurationChanged)

    /**
     * @brief Set the sample period
     *  The user does not need to change it, should be used internally
     *
     * @param sample_period
     */
    void set_sample_period(uint16_t sample_period)
    {
        if(sample_period != _sample_period) {
            _sample_period = sample_period;
            emit samplePeriodChanged();
        }
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
     * @return inter-sample period in seconds
     */
    double samplePeriod() { return _sample_period * _samplePeriodTickDuration; }

    /**
     * @brief return one-way range in meters
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
            _num_points = _firmwareMaxNumberOfPoints;
            _sample_period = calculateSamplePeriod(newRange);

            // reduce _sample period until we are within operational parameters
            // maximize the number of points
            while(_sample_period < _firmwareMinSamplePeriod) {
                _num_points--;
                _sample_period = calculateSamplePeriod(newRange);
            }

            emit numberOfPointsChanged();
            emit samplePeriodChanged();
            emit rangeChanged();
            emit transmitDurationMaxChanged();

            adjustTransmitDuration();
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
     * @brief Set speed of sound (m/s) used for calculating distance from time-of-flight
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
            _sample_period = calculateSamplePeriod(desiredRange);
            emit speedOfSoundChanged();
            emit samplePeriodChanged();
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

    /**
     * @brief Set the number of points
     *
     * @param num_points
     */
    void set_number_of_points(int num_points)
    {
        if(num_points != _num_points) {
            _num_points = num_points;
            emit numberOfPointsChanged();
            // Range uses number of points to calculate it, emit signal to update interface
            emit rangeChanged();
        }
    }
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
            // Reset reverse direction when back to full scan
            if(sectorSizeGrad == 400) {
                _reverse_direction = false;
            }
            _sectorSize = sectorSizeGrad;
            emit sectorSizeChanged();
        }
    }

    Q_PROPERTY(int sectorSize READ sectorSize WRITE setSectorSize NOTIFY sectorSizeChanged)

    /**
     * @brief The maximum transmit duration that will be applied is limited internally by the
     * firmware to prevent damage to the hardware
     * The maximum transmit duration is equal to 64 * the sample period in microseconds
     * @return the maximum transmit duration allowed in microseconds
     */
    int transmitDurationMax()
    {
        return std::min(_firmwareMaxTransmitDuration, static_cast<uint16_t>(samplePeriod() * 64e6));
    }
    Q_PROPERTY(int transmitDurationMax READ transmitDurationMax NOTIFY transmitDurationMaxChanged)

    /**
     * @brief Get frequency of the profile messages
     *
     * @return float
     */
    float profileFrequency()
    {
        return messageFrequencies[Ping360Id::DEVICE_DATA].frequency;
    }

    Q_PROPERTY(float profileFrequency READ profileFrequency NOTIFY messageFrequencyChanged)

    /**
     * @brief The maximum transmit duration that will be applied is limited internally by the
     * firmware to prevent damage to the hardware
     * The maximum transmit duration is equal to 64 * the sample period in microseconds
     * @return the maximum transmit duration allowed in microseconds
     */
    int transmitDurationMin()
    {
        return _firmwareMinTransmitDuration;
    }
    Q_PROPERTY(int transmitDurationMin READ transmitDurationMin CONSTANT)

    /**
     * @brief automatic transmit duration adjustment
     * @param automatic true to adjust the transmit duration automatically when the range is adjusted
     */
    void setAutoTransmitDuration(bool automatic)
    {
        if (_autoTransmitDuration == automatic) {
            return;
        }

        _autoTransmitDuration = automatic;
        emit autoTransmitDurationChanged();

        if (_autoTransmitDuration) {
            adjustTransmitDuration();
        }
    }

    /**
     * @brief automatic transmit duration adjustment
     * @return true if transmit duration is automatically adjusted when the range is changed
     */
    bool autoTransmitDuration() { return _autoTransmitDuration; }
    Q_PROPERTY(bool autoTransmitDuration READ autoTransmitDuration WRITE setAutoTransmitDuration NOTIFY
               autoTransmitDurationChanged)

    /**
     * @brief adjust the transmit duration according to automatic mode, and current configuration
     */
    void adjustTransmitDuration()
    {
        if (_autoTransmitDuration) {
            /*
             * Per firmware engineer:
             * 1. Starting point is TxPulse in usec = ((one-way range in metres) * 8000) / (Velocity of sound in metres per second)
             * 2. Then check that TxPulse is wide enough for currently selected sample interval in usec, i.e.,
             *    if TxPulse < (2.5 * sample interval) then TxPulse = (2.5 * sample interval)
             * 3. Perform limit checking
             */

            // 1
            int autoDuration = round(8000*range()/_speed_of_sound);
            // 2 (transmit duration is microseconds, samplePeriod() is nanoseconds)
            autoDuration = std::max(static_cast<int>(2.5*samplePeriod()/1000), autoDuration);
            // 3
            _transmit_duration = std::max(static_cast<int>(_firmwareMinTransmitDuration),
                                          std::min(transmitDurationMax(), autoDuration));
            emit transmitDurationChanged();
        } else if (_transmit_duration > transmitDurationMax()) {
            _transmit_duration = transmitDurationMax();
            emit transmitDurationChanged();
        }
    }

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

    /**
     * @brief A list of all availables baudrates to communicate with the sensor
     *
     * @return QVector<int>
     */
    const QList<int>& validBaudRates();
    Q_PROPERTY(QVariant validBaudRates READ validBaudRatesAsVariantList CONSTANT)

    /**
     * @brief Set the communication baud rate
     *
     * @param baudRate
     */
    Q_INVOKABLE void setBaudRate(int baudRate);

    /**
     * @brief Set the communication baud rate and request a profile
     *  This helps user front end changes in the baud rate
     *
     * @param baudRate
     */
    Q_INVOKABLE void setBaudRateAndRequestProfile(int baudRate);

    /**
     * @brief Start sensor configuration and baud rate detection
     *
     */
    Q_INVOKABLE void startConfiguration()
    {
        _configuring = true;
        if(_timeoutProfileMessage.isActive()) {
            _timeoutProfileMessage.stop();
        }
        _resetBaudRateDetection = true;
        checkBaudrateProcess();
    }

    /**
     * @brief Detect the best baudrate to use the highest possible
     *
     */
    void detectBaudrates();

signals:
    /**
     * @brief emitted when property changes
     */
///@{
    void angleChanged();
    void angleOffsetChanged();
    void angularSpeedChanged();
    void autoTransmitDurationChanged();
    void dataChanged();
    void gainSettingChanged();
    void messageFrequencyChanged();
    void numberOfPointsChanged();
    void pingNumberChanged();
    void reverseDirectionChanged();
    void samplePeriodChanged();
    void sectorSizeChanged();
    void rangeChanged();
    void speedOfSoundChanged();
    void transmitDurationChanged();
    void transmitDurationMaxChanged();
    void transmitFrequencyChanged();
///@}

private:
    Q_DISABLE_COPY(Ping360)
    /**
     * @brief Sensor variables
     */
///@{

    // firmware constants
    static const uint16_t _firmwareMaxNumberOfPoints;
    static const uint16_t _firmwareMaxTransmitDuration;
    static const uint16_t _firmwareMinTransmitDuration;
    static const uint16_t _firmwareMinSamplePeriod;
    // The firmware defaults at boot
    static const uint8_t _firmwareDefaultGainSetting;
    static const uint16_t _firmwareDefaultAngle;
    static const uint16_t _firmwareDefaultTransmitDuration;
    static const uint16_t _firmwareDefaultSamplePeriod;
    static const uint16_t _firmwareDefaultTransmitFrequency;
    static const uint16_t _firmwareDefaultNumberOfSamples;

    // The default transmit frequency to operate with
    static const uint16_t _viewerDefaultTransmitFrequency;
    static const uint16_t _viewerDefaultNumberOfSamples;

    // Physical properties of the sensor
    static const float _angularSpeedGradPerMs;

    uint16_t _angle = 200;
    uint16_t _transmit_duration = _firmwareDefaultTransmitDuration;
    uint32_t _gain_setting = _firmwareDefaultGainSetting;
    uint16_t _num_points = _viewerDefaultNumberOfSamples;
    uint16_t _sample_period = _firmwareDefaultSamplePeriod;
    uint16_t _transmit_frequency = _viewerDefaultTransmitFrequency;
    QVector<double> _data;
///@}

    // Number of messages to check for best baud rate
    // using Auto Baud Rate detection
    static const int _ABRTotalNumberOfMessages = 20;

    // _sample_period is the number of timer ticks between each data point
    // each timer tick has a duration of 25 nanoseconds
    static constexpr double _samplePeriodTickDuration = 25e-9;
    uint32_t _ping_number = 0;
    // Ping360 has a 200 offset by default
    int _angle_offset = 200;
    int _angular_speed = 1;
    bool _autoTransmitDuration = true;
    uint _central_angle = 1;
    bool _configuring = true;
    // Number of messages used to check the best baud rate
    int _preConfigurationTotalNumberOfMessages = 20;
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
    // The sensor will reset the position after 30s without communication
    int _sensorRestartTimeoutMs = 30000;

    // Sector size in gradians, default is full circle
    int _sectorSize = 400;

    QTimer _messageFrequencyTimer;
    QTimer _timeoutProfileMessage;

    /**
     * @brief This timer allows us to wait for a couple of seconds for an answer.
     *  If the baudrate is not valid or the sensor is unable to communicate with us because of noise or something else,
     *  we should trigger the request again, since it forces the request to be made by our side.
     *  Without this timer, we are going to wait forever for a reply without asking again.
     */
    QTimer _baudrateConfigurationTimer;
    bool _resetBaudRateDetection = true;

    // Helper structure to hold frequency information for each message
    struct MessageFrequencyHelper {
        // Hold last frequency
        float frequency;
        // Hold the last number of messages
        int numberOfMessages = 0;

        /**
         * @brief Increase the number of messages parsed
         *
         * @param number of messages
         */
        void updateNumberOfMessages(int number = 1)
        {
            numberOfMessages += number;
        }

        /**
         * @brief Update frequency of structure with the elapsed time between calls
         *
         * @param milliseconds elapsed number of milliseconds since last call
         */
        float updateFrequencyFromMilliseconds(float milliseconds)
        {
            if(qFuzzyIsNull(milliseconds) || std::isinf(milliseconds) || std::isnan(milliseconds)) {
                return 0;
            }
            // Update frequency
            frequency = static_cast<float>(numberOfMessages)/(milliseconds*0.001f);

            // Clear number of messages to the next call
            numberOfMessages = 0;

            return frequency;
        }
    };

    QHash<uint16_t, MessageFrequencyHelper> messageFrequencies;

    void handleMessage(const ping_message& msg) final; // handle incoming message

    void loadLastSensorConfigurationSettings();
    void updateSensorConfigurationSettings();
    void setLastSensorConfiguration();

    const QVariantList& validBaudRatesAsVariantList() const;
    QVariantList _validBaudRates = {
        2000000,
        921600,
        570600,
        460800,
        257600,
        250000,
        230400,
        115200,
    };

    /**
     * @brief Calculate sample period from distance
     *
     * @param distance
     * @return uint16_t
     */
    uint16_t calculateSamplePeriod(float distance);

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
     * @brief Request and calculate the necessary amount of messages to get the best baud rate possible
     *  You can check the following description to understand how this is done.
     *  1 - A timer is used with the timeout of 100ms to wait for the `CommonId::DEVICE_INFORMATION` message.
     *          The objective of this timer is to not wait forever, the sensor plus the communication channel latency
     *          has a 100ms window to reply the message, if the message is not answered inside this timeout window,
     *          the procedure will check that as a communication failure and a negative point will be added in this baud
     *          rate.
     *  2 - After the total number of messages `_ABRTotalNumberOfMessages` for a specific baud rate is done,
     *          the procedure will check if the total number of messages are valid, and if its, this baud rate will be
     *          used to do the communication between ping-viewer and the sensor. But, if the baud rate is not valid
     *          and a single message is lost, the function will change the baud rate to a lower frequency and will
     *          restart the check again for the new baud rate.
     * 3 - If a single baud rate is not valid, the lowest one will be used.
     *
     */
    void checkBaudrateProcess();

    /**
     * @brief Stop necessary timers and variables that deals with sensor configuration
     *
     */
    void stopConfiguration();

    /**
     * @brief Take care of github payload and detect new versions available
     *
     * @param jsonDocument
     */
    void checkNewFirmwareInGitHubPayload(const QJsonDocument& jsonDocument);
};
