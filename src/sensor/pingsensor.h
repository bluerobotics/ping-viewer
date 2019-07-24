#pragma once

#include "sensor.h"

/**
 * @brief Abstract ping sensors
 *
 */
class PingSensor : public Sensor
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new Ping Sensor object
     *
     */
    PingSensor();

    /**
     * @brief Destroy the Ping Sensor object
     *
     */
    ~PingSensor();

    /**
     * @brief debug function
     */
    virtual void printStatus() const;

    /**
     * @brief Add new connection
     *
     * @param connType connection type
     * @param connString arguments for the new connection
     */
    Q_INVOKABLE virtual void connectLink(AbstractLinkNamespace::LinkType connType, const QStringList& connString) = 0;

    /**
     * @brief Get device source ID
     *
     * @return uint8_t source id
     */
    uint8_t srcId() const { return _srcId; }
    Q_PROPERTY(int srcId READ srcId NOTIFY srcIdUpdate)

    /**
     * @brief Return destination ID
     *
     * @return uint8_t destination ID
     */
    uint8_t dstId() const { return _dstId; }
    Q_PROPERTY(int dstId READ dstId NOTIFY dstIdUpdate)

    //TODO: move functions from snake case to camel case
    /**
     * @brief Return firmware major version
     *
     * @return uint8_t firmware major version number
     */
    uint8_t firmware_version_major() const { return _firmware_version_major; }
    Q_PROPERTY(int firmware_version_major READ firmware_version_major NOTIFY firmwareVersionMajorUpdate)

    /**
     * @brief Return firmware minor version
     *
     * @return uint8_t firmware minor version number
     */
    uint16_t firmware_version_minor() const { return _firmware_version_minor; }
    Q_PROPERTY(int firmware_version_minor READ firmware_version_minor NOTIFY firmwareVersionMinorUpdate)

    /**
     * @brief Return firmware patch version
     *
     * @return uint8_t firmware patch version number
     */
    uint16_t firmware_version_patch() const { return _firmware_version_patch; }
    Q_PROPERTY(int firmware_version_patch READ firmware_version_patch NOTIFY firmwareVersionPatchUpdate)

    /**
     * @brief Return device type number
     *
     * @return uint8_t Device type number
     */
    uint8_t device_type() const { return _device_type; }
    Q_PROPERTY(int device_type READ device_type NOTIFY deviceTypeUpdate)

    /**
     * @brief Return device model number
     *
     * @return uint8_t Device model number
     */
    uint8_t device_revision() const { return _device_revision; }
    Q_PROPERTY(int device_revision READ device_revision NOTIFY deviceRevisionUpdate)

    /**
     * @brief Return last ascii_text message
     *
     * @return QString
     */
    QString asciiText() const { return _ascii_text; }
    Q_PROPERTY(QString ascii_text READ asciiText NOTIFY asciiTextUpdate)

    /**
     * @brief Return last err_msg message
     * TODO: change function name
     *
     * @return QString
     */
    QString errMsg() const { return _nack_msg; }
    Q_PROPERTY(QString err_msg READ errMsg NOTIFY nackMsgUpdate)

    /**
     * @brief Return number of parser errors
     *
     * @return int
     */
    int parserErrors() const { return _parser ? _parser->errors : 0; }
    Q_PROPERTY(int parser_errors READ parserErrors NOTIFY parserErrorsUpdate)

    /**
     * @brief Return number of successfully parsed messages
     *
     * @return int
     */
    int parsedMsgs() const { return _parser ? _parser->parsed : 0; }
    Q_PROPERTY(int parsed_msgs READ parsedMsgs NOTIFY parsedMsgsUpdate)

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
    Q_INVOKABLE virtual void request(int id) const;

    /**
     * @brief Do firmware sensor update
     *
     * @param fileUrl firmware file path
     * @param sendPingGotoBootloader Use "goto bootloader" message
     * @param baud baud rate value
     * @param verify this variable is true when all
     */
    Q_INVOKABLE virtual void firmwareUpdate(QString fileUrl, bool sendPingGotoBootloader = true, int baud = 57600,
                                            bool verify = true) = 0;

signals:
    //TODO: Move from Update to Changed
    void asciiTextUpdate();
    void deviceRevisionUpdate();
    void deviceTypeUpdate();
    void dstIdUpdate();
    void firmwareVersionMajorUpdate();
    void firmwareVersionMinorUpdate();
    void firmwareVersionPatchUpdate();
    void lostMessagesUpdate();
    void nackMsgUpdate();
    void parsedMsgsUpdate();
    void parserErrorsUpdate();
    void protocolVersionMajorUpdate();
    void protocolVersionMinorUpdate();
    void protocolVersionPatchUpdate();
    void srcIdUpdate();

protected:
    /**
     * @brief Handle new ping protocol messages
     *
     * @param msg
     */
    void handleMessagePrivate(const ping_message& msg);

    /**
     * @brief Handle new ping protocol messages
     *
     * @param msg
     */
    virtual void handleMessage(const ping_message& msg) { Q_UNUSED(msg) };

    /**
     * @brief Print specific information about a specific sensor
     *  Information will be printed with pingStatus
     */
    virtual void printSensorInformation() const = 0;

    /**
     * @brief Write a message to link
     *
     * @param msg
     */
    void writeMessage(const ping_message& msg) const;

    QString _ascii_text;
    uint8_t _device_revision{0};
    uint8_t _device_type{0};
    uint8_t _dstId{0};
    uint8_t _firmware_version_major{0};
    uint8_t _firmware_version_minor{0};
    uint8_t _firmware_version_patch{0};
    int _lostMessages{0};
    QString _nack_msg;
    uint8_t _protocol_version_major{0};
    uint8_t _protocol_version_minor{0};
    uint8_t _protocol_version_patch{0};
    uint8_t _srcId{0};

private:
    Q_DISABLE_COPY(PingSensor)
};
