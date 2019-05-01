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
     *          note: This should be received by copy, since we could use our own link to connect
     * @param logConf log configuration
     *
     */
    void connectLink(const LinkConfiguration conConf, const LinkConfiguration& logConf = LinkConfiguration());

    /**
     * @brief Add new log connection
     *
     * @param logConf log configuration
     */
    void connectLinkLog(const LinkConfiguration& logConf);

    /**
     * @brief Return true if sensor is connected
     *
     * @return bool
     */
    bool connected() { return _connected; };
    Q_PROPERTY(bool connected READ connected NOTIFY connectionUpdate)


    /**
     * @brief Return the list of firmwares available to download
     *
     * @return QMap<QString, QVariant>
     */
    QMap<QString, QVariant> firmwaresAvailable() { return _firmwares; };
    Q_PROPERTY(QMap<QString, QVariant> firmwaresAvailable READ firmwaresAvailable NOTIFY firmwaresAvailableUpdate)

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
    bool _connected;
    // For now this will be structures by: firmware file name, and remote address
    // TODO: A Model should be created to handle this for us
    QMap<QString, QVariant> _firmwares;
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
    void firmwaresAvailableUpdate();
    void linkUpdate();
    void nameUpdate();

    // Out
    void linkLogUpdate();

private:
    Q_DISABLE_COPY(Sensor)
};
