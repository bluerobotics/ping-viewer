#pragma once

#include <QPointer>
#include <QQmlComponent>
#include <QQuickItem>

#include "flasher.h"
#include "link.h"
#include "logger.h"
#include "parser.h"
#include "protocoldetector.h"
#include "sensorinfo.h"

/**
 * @brief Manage sensor connection
 *
 */
class Sensor : public QObject {
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
    Q_PROPERTY(AbstractLink* link READ link NOTIFY linkChanged)

    /**
     * @brief Return log link
     *
     * @return AbstractLink*
     */
    AbstractLink* linkLog() const { return _linkOut.data() ? _linkOut->self() : nullptr; };
    Q_PROPERTY(AbstractLink* linkLog READ linkLog NOTIFY linkLogChanged)

    /**
     * @brief Return sensor name
     *
     * @return QString
     */
    QString name() const { return _name; };
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)

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
    Q_PROPERTY(bool connected READ connected NOTIFY connectionChanged)

    /**
     * @brief Return the list of firmwares available to download
     *
     * @return QMap<QString, QVariant>
     */
    QMap<QString, QVariant> firmwaresAvailable() const { return _firmwares; };
    Q_PROPERTY(QMap<QString, QVariant> firmwaresAvailable READ firmwaresAvailable NOTIFY firmwaresAvailableChanged)

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
     * @brief Reset sensor configuration
     *
     */
    Q_INVOKABLE virtual void resetSettings();

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
     * @brief Set the sensor status model url
     *
     * @param url
     */
    void setSensorStatusModel(const QUrl& url);

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

    /**
     * @brief Return a qml component that will take care of sensor status
     *
     * @param parent
     * @return QQuickItem* sensorStatusModel
     */
    Q_INVOKABLE virtual QQuickItem* sensorStatusModel(QObject* parent = nullptr);

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

    QString _name;

    // Hold sensor information of the class
    SensorInfo _sensorInfo;

    // Hold the sensor visualizer item
    QSharedPointer<QQuickItem> _sensorVisualizer;
    QUrl _sensorVisualizerUrl;

    // Hold the sensor status visualizer item
    QSharedPointer<QQuickItem> _sensorStatusModel;
    QUrl _sensorStatusModelUrl;

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

    /**
     * @brief Set the sensor name
     *
     * @param name
     */
    void setName(const QString& name);

signals:
    void autoDetectUpdate(bool autodetect);

    // In
    void connectionClose();
    void connectionOpen();
    void connectionChanged();
    void firmwaresAvailableChanged();
    void linkChanged();
    void nameChanged();

    // Out
    void linkLogChanged();

private:
    Q_DISABLE_COPY(Sensor)
};
