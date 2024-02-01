#pragma once

#include <algorithm>
#include <cmath>
#include <functional>

#include <QElapsedTimer>
#include <QProcess>
#include <QTimer>

#include "mavlinkmanager.h"
#include "parser.h"
#include "ping-message-common.h"
#include "ping-message-ping360.h"
#include "ping360bootloaderpacket.h"
#include "pingparserext.h"
#include "pingsensor.h"
#include "protocoldetector.h"

/**
 * @brief Define Ping360 sensor
 * Ping 360 Sonar
 *
 */
class Ping360 : public PingSensor {
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
     * @brief Helper function to request profiles from sensor based in the actual position
     *
     * @param delta number of grads/steps from the actual position
     * @param transmit request profile data
     */
    Q_INVOKABLE void deltaStep(int delta, bool transmit = true)
    {
        // Force nextPoint to be positive and inside our polar space
        int nextPoint = _angle + delta;
        while (nextPoint < 0) {
            nextPoint += _angularResolutionGrad;
        }
        nextPoint %= _angularResolutionGrad;

        transducer_message.set_mode(1);
        transducer_message.set_gain_setting(_sensorSettings.gain_setting);
        transducer_message.set_angle(nextPoint);
        transducer_message.set_transmit_duration(_sensorSettings.transmit_duration);
        transducer_message.set_sample_period(_sensorSettings.sample_period);
        transducer_message.set_transmit_frequency(_sensorSettings.transmit_frequency);
        transducer_message.set_number_of_samples(_sensorSettings.num_points);
        transducer_message.set_transmit(transmit);

        transducer_message.updateChecksum();
        writeMessage(transducer_message);
    }

    /**
     * @brief Return number of pings emitted
     *
     * @return uint32_t
     */
    uint32_t ping_number() const { return _ping_number; }
    Q_PROPERTY(int ping_number READ ping_number NOTIFY pingNumberChanged)

    /**
     * @brief Set pulse emission in ms
     *
     * @param transmit_duration
     */
    void set_transmit_duration(int transmit_duration)
    {
        if (_sensorSettings.transmit_duration != transmit_duration) {
            _sensorSettings.transmit_duration = transmit_duration;
            emit transmitDurationChanged();
        }
    }

    /**
     * @brief Return pulse emission in ms
     *
     * @return uint16_t
     */
    uint16_t transmit_duration() const { return _sensorSettings.transmit_duration; }
    Q_PROPERTY(int transmit_duration READ transmit_duration WRITE set_transmit_duration NOTIFY transmitDurationChanged)

    /**
     * @brief Set the sample period
     *  The user does not need to change it, should be used internally
     *
     * @param sample_period
     */
    void set_sample_period(uint16_t sample_period)
    {
        if (_sensorSettings.sample_period != sample_period) {
            _sensorSettings.sample_period = sample_period;
            emit samplePeriodChanged();
        }
    }

    /**
     * @brief Return the sample period in ms
     *
     * @return int
     */
    int sample_period() const { return _sensorSettings.sample_period; }
    Q_PROPERTY(int sample_period READ sample_period NOTIFY samplePeriodChanged)

    /**
     * @brief Set the transmit frequency in Hz
     *
     * @param transmit_frequency
     */
    void set_transmit_frequency(int transmit_frequency)
    {
        _sensorSettings.transmit_frequency = transmit_frequency;
        emit transmitFrequencyChanged();
    }

    /**
     * @brief Return the transmit frequency in Hz
     *
     * @return int
     */
    int transmit_frequency() const { return _sensorSettings.transmit_frequency; }
    Q_PROPERTY(
        int transmit_frequency READ transmit_frequency WRITE set_transmit_frequency NOTIFY transmitFrequencyChanged)

    /**
     * @brief Angle of sensor head in gradians (400)
     *
     * @return uint16_t
     */
    uint16_t angle()
    {
        // Only use heading correction if running in full scam mode (sector size == resolution)
        const int angle
            = _angle + angle_offset() + (_sectorSize == _angularResolutionGrad ? static_cast<int>(_heading) : 0);
        return angle % _angularResolutionGrad;
    }
    Q_PROPERTY(int angle READ angle NOTIFY angleChanged)

    /**
     * @brief The sonar communicates the sample_period in units of 25nsec ticks
     * @return inter-sample period in seconds
     */
    double samplePeriod() const { return _sensorSettings.sample_period * _samplePeriodTickDuration; }

    /**
     * @brief return one-way range in meters
     *
     * @return uint32_t
     */
    double range() const { return samplePeriod() * _sensorSettings.num_points * _speed_of_sound / 2; }

    /**
     * @brief Set sensor window analysis size
     *
     * @param newRange
     */
    void set_range(double newRange)
    {
        if (qFuzzyCompare(newRange, range())) {
            return;
        }

        _sensorSettings.num_points = _firmwareMaxNumberOfPoints;
        _sensorSettings.sample_period = calculateSamplePeriod(newRange);

        // reduce _sample period until we are within operational parameters
        // maximize the number of points
        while (_sensorSettings.sample_period < _firmwareMinSamplePeriod) {
            _sensorSettings.num_points--;
            _sensorSettings.sample_period = calculateSamplePeriod(newRange);
        }

        emit numberOfPointsChanged();
        emit samplePeriodChanged();
        emit rangeChanged();
        emit transmitDurationMaxChanged();

        adjustTransmitDuration();
    }
    Q_PROPERTY(double range READ range WRITE set_range NOTIFY rangeChanged)

    /**
     * @brief Return gain setting
     *
     * @return uint32_t
     */
    uint32_t gain_setting() const { return _sensorSettings.gain_setting; }

    /**
     * @brief Set sensor gain index
     *
     * @param gain_setting
     */
    void set_gain_setting(int gain_setting)
    {
        _sensorSettings.gain_setting = gain_setting;
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
    uint32_t speed_of_sound() const { return _speed_of_sound; }

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
            _sensorSettings.sample_period = calculateSamplePeriod(desiredRange);
            emit speedOfSoundChanged();
            emit samplePeriodChanged();
            emit rangeChanged();
        }
    }
    Q_PROPERTY(int speed_of_sound READ speed_of_sound WRITE set_speed_of_sound NOTIFY speedOfSoundChanged)

    /**
     * @brief Returns the angle offset from sample position
     *
     * @return int
     */
    int angle_offset() const { return _angle_offset; }

    /**
     * @brief Set angle offset from sample position
     *
     * @param angle_offset
     */
    void set_angle_offset(int angle_offset)
    {
        if (angle_offset != _angle_offset) {
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
    int angular_speed() const { return _angular_speed; }

    /**
     * @brief Set the angular speed
     *
     * @param angular_speed
     */
    void set_angular_speed(int angular_speed)
    {
        if (angular_speed != _angular_speed) {
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
    bool reverse_direction() const { return _reverse_direction; }

    /**
     * @brief Set reverse direction
     *
     * @param reverse_direction
     */
    void set_reverse_direction(bool reverse_direction)
    {
        if (reverse_direction != _reverse_direction) {
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
    int number_of_points() const { return _sensorSettings.num_points; }

    /**
     * @brief Set the number of points
     *
     * @param num_points
     */
    void set_number_of_points(int num_points)
    {
        if (_sensorSettings.num_points != num_points) {
            _sensorSettings.num_points = num_points;
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
    int sectorSize() const { return round(_sectorSize * 360 / 400.0); }

    /**
     * @brief Set sector size in degrees
     *
     * @param sectorSize
     */
    void setSectorSize(int sectorSize)
    {
        int sectorSizeGrad = round(sectorSize * 400 / 360.0);

        if (_sectorSize != sectorSizeGrad) {
            // Reset reverse direction when back to full scan
            if (sectorSizeGrad == 400) {
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
    float profileFrequency() const;

    Q_PROPERTY(float profileFrequency READ profileFrequency NOTIFY messageFrequencyChanged)

    /**
     * @brief The maximum transmit duration that will be applied is limited internally by the
     * firmware to prevent damage to the hardware
     * The maximum transmit duration is equal to 64 * the sample period in microseconds
     * @return the maximum transmit duration allowed in microseconds
     */
    int transmitDurationMin() const { return _firmwareMinTransmitDuration; }
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
    bool autoTransmitDuration() const { return _autoTransmitDuration; }
    Q_PROPERTY(bool autoTransmitDuration READ autoTransmitDuration WRITE setAutoTransmitDuration NOTIFY
            autoTransmitDurationChanged)

    /**
     * @brief Sensor origin orientation in gradians (400)
     *
     * @return float
     */
    float heading() const { return _heading; }
    Q_PROPERTY(float heading READ heading NOTIFY headingChanged)

    /**
     * @brief flag is true if the device is in bootloader mode
     */
    bool isBootloader() { return _isBootloader; }
    Q_PROPERTY(bool isBootloader READ isBootloader NOTIFY isBootloaderChanged)

    /**
     * @brief adjust the transmit duration according to automatic mode, and current configuration
     */
    void adjustTransmitDuration()
    {
        if (_autoTransmitDuration) {
            /*
             * Per firmware engineer:
             * 1. Starting point is TxPulse in usec = ((one-way range in metres) * 8000) / (Velocity of sound in metres
             * per second)
             * 2. Then check that TxPulse is wide enough for currently selected sample interval in usec, i.e.,
             *    if TxPulse < (2.5 * sample interval) then TxPulse = (2.5 * sample interval)
             * 3. Perform limit checking
             */

            // 1
            int autoDuration = round(8000 * range() / _speed_of_sound);
            // 2 (transmit duration is microseconds, samplePeriod() is nanoseconds)
            autoDuration = std::max(static_cast<int>(2.5 * samplePeriod() / 1000), autoDuration);
            // 3
            _sensorSettings.transmit_duration = std::max(
                static_cast<int>(_firmwareMinTransmitDuration), std::min(transmitDurationMax(), autoDuration));
            emit transmitDurationChanged();
        } else if (_sensorSettings.transmit_duration > transmitDurationMax()) {
            _sensorSettings.transmit_duration = transmitDurationMax();
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
    Q_INVOKABLE void firmwareUpdate(
        QString fileUrl, bool sendPingGotoBootloader = true, int baud = 57600, bool verify = true) final override;

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
        if (_timeoutProfileMessage.isActive()) {
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

    /**
     * @brief Enable or disable heading integration
     *
     * @param enable
     */
    Q_INVOKABLE void enableHeadingIntegration(bool enable);

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
    void headingChanged();
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
    void isBootloaderChanged();
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
    static const float _viewerDefaultRange;
    static const uint16_t _viewerDefaultSamplePeriod;
    static const uint32_t _viewerDefaultSpeedOfSound;
    static const uint16_t _viewerDefaultTransmitDuration;

    // Physical properties of the sensor
    static const float _angularSpeedGradPerMs;

    /** Tracks the interface settings for the sensor
     * This structure matches the basic configuration for Ping360
     * The `valid` field helps to invalidate the sensor configuration
     * forcing our class to set and send the profile request with our new settings.
     *
     * Why is this necessary ?
     * Ping360 works with an ping-pong strategy, this is a result of the half-duplex
     * communication nature.
     * We should track and reset the sensor settings, since profile messages can be in
     * the buffer or in the way to be decoded while we set the new settings.
     * This can result in two behaviours:
     *     - A new profile message is sent while a second one is being processed
     *          resulting in two different profiles requests making the sensor
     *          to go further and back in position for each message.
     *     - The new settings can be overwritten by the incoming profile message,
     *          resulting in the the normal behaviour with the old settings.
     */
    struct Ping360Settings {
        uint16_t transmit_duration = _firmwareDefaultTransmitDuration;
        uint32_t gain_setting = _firmwareDefaultGainSetting;
        uint16_t num_points = _viewerDefaultNumberOfSamples;
        uint16_t sample_period = _viewerDefaultSamplePeriod;
        uint16_t transmit_frequency = _viewerDefaultTransmitFrequency;

        uint16_t start_angle = 0;
        uint16_t end_angle = 399;
        bool checkSector = false;

        bool valid = true;

        /**
         * @brief Check if input is valid, if so update valid variable
         *
         * @param other
         * @return true
         * @return false
         */
        bool checkValidation(const Ping360Settings& other)
        {
            valid = other.transmit_duration == transmit_duration && other.gain_setting == gain_setting
                && other.num_points == num_points && other.sample_period == sample_period
                && other.transmit_frequency == transmit_frequency;
            if (checkSector && valid) {
                valid = other.start_angle == start_angle && other.end_angle == end_angle;
            }
            return valid;
        }
    } _sensorSettings;

    // This variables are not user configuration settings
    uint16_t _angle = 200;
    QVector<double> _data;
    ///@}

    /**
     * @brief Struct to deal with profile request logic and sensor state
     *
     */
    struct Ping360RequestStateStruct {
        enum class Type {
            /**
             * @brief Legacy request logic
             *  It's based in a sync ping-pong request-reply approach,
             *  each request creates a single reply.
             */
            Legacy,

            /**
             * @brief Async request logic
             *  It's based in async requests for the profiles message (DEVICE_DATA [2300]),
             *  a request can create multiple replies.
             *  New `auto_transmit` message.
             */
            AutoTransmitAsync,
        };

        Type type = Type::Legacy;
    } _profileRequestLogic;

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

    static constexpr int _angularResolutionGrad = 400;
    // The motor takes 4s to run a full circle
    static constexpr float _motorSpeedGradMs = 4000 / _angularResolutionGrad;
    // Right now the max value is 1200 for ping360
    // We are saving 2k of the memory for future proof modifications
    static constexpr int _maxNumberOfPoints = 2048;
    // The sensor can take 4s to answer, we are also using an extra 200ms for latency
    static constexpr int _sensorTimeout = 4200;
    // The sensor will reset the position after 30s without communication
    static constexpr int _sensorRestartTimeoutMs = 30000;

    // Sector size in gradians, default is full circle
    int _sectorSize = 400;

    // Sensor heading in radians
    float _heading = 0;

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
        void updateNumberOfMessages(int number = 1) { numberOfMessages += number; }

        /**
         * @brief Update frequency of structure with the elapsed time between calls
         *
         * @param milliseconds elapsed number of milliseconds since last call
         */
        float updateFrequencyFromMilliseconds(float milliseconds)
        {
            if (qFuzzyIsNull(milliseconds) || std::isinf(milliseconds) || std::isnan(milliseconds)) {
                return 0;
            }
            // Update frequency
            frequency = static_cast<float>(numberOfMessages) / (milliseconds * 0.001f);

            // Clear number of messages to the next call
            numberOfMessages = 0;

            return frequency;
        }
    };

    /* Used to improve execution time in frequently function calls
     * avoiding message unnecessary constructions and desconstruction
     */
    ping360_transducer transducer_message;

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
     * @brief Legacy profile request
     *  Used in firmwares 3.1
     *
     */
    void legacyProfileRequest();

    /**
     * @brief Async profile request
     *  Used in firmwares from 3.2
     *
     */
    void asyncProfileRequest();

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

    /**
     * @brief Process incoming mavlink messages
     *
     * @param message
     */
    void processMavlinkMessage(const mavlink_message_t& message);

    /**
     * @brief Check the link to see if the sonar is stuck in the bootloader
     * this is performed once when the link is opened
     */
    void checkBootloader();

    /**
     * @brief This is used to probe/communicate with the bootloader
     */
    Ping360BootloaderPacket _ping360BootloaderPacketParser;

    /**
     * @brief This is used to flag the qml so that the user can be notified that the
     * device is stuck in the bootloader
     */
    bool _isBootloader = false;
};
