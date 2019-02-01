#pragma once

#include <QPointer>

#include "flasher.h"
#include "link.h"
#include "parsers/parser.h"
#include "protocoldetector.h"

// TODO: rename to Device?
/**
 * @brief Manage sensor connection
 *
 */
class Sensor : public QObject
{
    Q_OBJECT
public:
    Sensor();
    ~Sensor();

    /**
     * @brief Return entry link
     *
     * @return AbstractLink*
     */
    AbstractLink* link() { return _linkIn.data() ? _linkIn->self() : nullptr; };
    Q_PROPERTY(AbstractLink* link READ link NOTIFY linkUpdate)

    /**
     * @brief Return log link
     *
     * @return AbstractLink*
     */
    AbstractLink* linkLog() { return _linkOut.data() ? _linkOut->self() : nullptr; };
    Q_PROPERTY(AbstractLink* linkLog READ linkLog NOTIFY linkLogUpdate)

    /**
     * @brief Return sensor name
     *
     * @return QString
     */
    QString name() { return _name; };
    Q_PROPERTY(QString name READ name NOTIFY nameUpdate)

    /**
     * @brief Add new connection and log
     *
     * @param conConf connection configuration
     * @param logConf log configuration
     */
    void connectLink(const LinkConfiguration& conConf, const LinkConfiguration& logConf = LinkConfiguration());

    /**
     * @brief Add new log connection
     *
     * @param logConf log configuration
     */
    void connectLinkLog(const LinkConfiguration& logConf);

    /**
     * @brief Set auto detect
     *
     * @param autodetect
     */
    void setAutoDetect(bool autodetect);
    /**
     * @brief Check auto detect state
     *
     * @return true running
     * @return false not running
     */
    bool autoDetect() { return _autodetect; };
    Q_PROPERTY(bool autodetect READ autoDetect WRITE setAutoDetect NOTIFY autoDetectUpdate)

    /**
     * @brief Return true if sensor is connected
     *
     * @return bool
     */
    bool connected() { return _connected; };
    Q_PROPERTY(bool connected READ connected NOTIFY connectionUpdate)

    /**
     * @brief Return protocol detector
     *
     * @return const ProtocolDetector*
     */
    ProtocolDetector* detector() { return _detector; };

    /**
     * @brief Return protocol detector thread
     *
     * @return const QThread*
     */
    QThread* detectorThread() { return &_detectorThread; };


    /**
     * @brief Return flasher class used by this sensor
     *  TODO: This should be moved to a singleton flasher instance
     *  But to do such thing, is necessary a DeviceManager to deal with multiple devices and to manage the flash
     *   procedure.
     *
     * @return Flasher*
     */
    Flasher* flasher() { return &_flasher; };
    Q_PROPERTY(Flasher* flasher READ flasher CONSTANT)


protected:
    bool _autodetect;
    bool _connected;
    ProtocolDetector* _detector;
    QThread _detectorThread;
    // This class should be a singleton that will work with the future DeviceManager class
    // TODO: Move to a singleton and integrate with DeviceManager
    Flasher _flasher;
    QSharedPointer<Link> _linkIn;
    QSharedPointer<Link> _linkOut;
    Parser* _parser; // communication implementation

    QString _name; // TODO: populate

signals:
    void autoDetectUpdate(bool autodetect);

    // In
    void connectionClose();
    void connectionOpen();
    void connectionUpdate();
    void nameUpdate();
    void linkUpdate();

    // Out
    void linkLogUpdate();

private:
    Q_DISABLE_COPY(Sensor)
};
