#pragma once

#include <QQmlComponent>
#include <QQuickItem>
#include <QPointer>

#include "flasher.h"
#include "link.h"
#include "parser.h"
#include "protocoldetector.h"
#include "sensorinfo.h"

// TODO: rename to Device?
/**
 * @brief Manage sensor connection
 *
 */
class Sensor : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new Sensor object
     *  This class is an abstraction layer for different sensors
     *
     * @param sensorInfo
     */
    Sensor(SensorInfo sensorInfo);
    ~Sensor();

    /**
     * @brief Return entry link
     *
     * @return AbstractLink*
     */
    AbstractLink* link() const { return _linkIn.data() ? _linkIn->self() : nullptr; };
    Q_PROPERTY(AbstractLink* link READ link NOTIFY linkUpdate)

    /**
     * @brief Return log link
     *
     * @return AbstractLink*
     */
    AbstractLink* linkLog() const { return _linkOut.data() ? _linkOut->self() : nullptr; };
    Q_PROPERTY(AbstractLink* linkLog READ linkLog NOTIFY linkLogUpdate)

    /**
     * @brief Return sensor name
     *
     * @return QString
     */
    QString name() const { return _name; };
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
    bool connected() const { return _connected; };
    Q_PROPERTY(bool connected READ connected NOTIFY connectionUpdate)


    /**
     * @brief Return the list of firmwares available to download
     *
     * @return QMap<QString, QVariant>
     */
    QMap<QString, QVariant> firmwaresAvailable() const { return _firmwares; };
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

    /**
     * @brief Set the control panel url
     *
     * @param url
     */
    void setControlPanel(const QUrl& url);

    /**
     * @brief Set the sensor visualizer url
     *
     * @param url
     */
    void setSensorVisualizer(const QUrl& url);

    /**
     * @brief Return a qml component that will take care of the main control panel for the sensor
     *
     * @param parent
     * @return QQuickItem* controlPanel
     */
    Q_INVOKABLE virtual QQuickItem* controlPanel(QObject* parent = nullptr);

    /**
     * @brief Return a qml component that will take care of the main visualization widget
     * @param parent
     * @return QQuickItem* sensorVisualizer
     */
    Q_INVOKABLE virtual QQuickItem* sensorVisualizer(QObject* parent = nullptr);


protected:
    bool _connected;
    // Hold the control panel item
    QSharedPointer<QQuickItem> _controlPanel;
    QUrl _controlPanelUrl;
    static const QUrl _defaultControlPanelUrl;

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

    // Hold sensor information of the class
    SensorInfo _sensorInfo;

    // Hold the sensor visualizer item
    QSharedPointer<QQuickItem> _sensorVisualizer;
    QUrl _sensorVisualizerUrl;

    /**
     * @brief Helper function to create QQuickItems
     *
     * @param parent
     * @param resource
     * @param pointerQuickItem
     * @return true QQuickItem is accessible
     * @return false QQuickItem is not accessible or created
     */
    bool createQQuickItem(QObject* parent, const QUrl& resource, QSharedPointer<QQuickItem>& pointerQuickItem);

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
