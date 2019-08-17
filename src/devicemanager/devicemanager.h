#pragma once

#include <QAbstractListModel>
#include <QLoggingCategory>
#include <QThread>

#include "link/abstractlinknamespace.h"

#include "sensor/ping360ethernetfinder.h"
#include "sensor/protocoldetector.h"
#include "sensor/sensor.h"

Q_DECLARE_LOGGING_CATEGORY(DEVICEMANAGER)

class QJSEngine;
class QQmlEngine;

Q_DECLARE_METATYPE(QSharedPointer<Sensor>)
Q_DECLARE_METATYPE(LinkConfiguration*)

/**
 * @brief Manage the project DeviceManager
 *
 */
class DeviceManager : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
     * @brief Return DeviceManager pointer
     *
     * @return DeviceManager*
     */
    static DeviceManager* self();
    ~DeviceManager();

    /**
     * @brief Return a pointer of this singleton to the qml register function
     *
     * @param engine
     * @param scriptEngine
     * @return QObject*
     */
    static QObject* qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine);

    /**
     * @brief Enables protocol detector
     *
     */
    Q_INVOKABLE void startDetecting();

    /**
     * @brief Disables protocol detector
     *
     */
    Q_INVOKABLE void stopDetecting();

    /**
     * @brief Create a sensor object with the desired link configuration
     *  This sensor object will be available via primarySensor
     *
     * @param linkConf
     */
    Q_INVOKABLE void connectLink(LinkConfiguration* linkConf);

    /**
     * @brief Used to connect with link directly if it was not detected by the protocol detector
     *
     * @param connType
     * @param connString
     * @param deviceType
     */
    Q_INVOKABLE void connectLinkDirectly(AbstractLinkNamespace::LinkType connType, const QStringList& connString,
                                         PingEnumNamespace::PingDeviceType deviceType);

    /**
     * @brief Play a log file
     *
     * @param connType
     * @param connString
     */
    Q_INVOKABLE void playLogFile(AbstractLinkNamespace::LinkType connType, const QStringList& connString);

    /**
     * @brief Append configuration to the list of available connections
     *
     * @param linkConf
     * @param deviceName
     */
    void append(const LinkConfiguration& linkConf, const QString& deviceName = "None");

    /**
     * @brief Returns the data stored under the given role for the item referred to by the index
     *
     * @param index
     * @param role
     * @return QVariant
     */
    QVariant data(const QModelIndex& index, int role) const override
    {
        const int indexRow = index.row();
        QVector<QVariant> vectorRole = _sensors[role];

        if(indexRow < 0 || vectorRole.size() <= indexRow) {
            return {};
        }

        // Return a QVariant<pointer>
        if(role == Connection) {
            return QVariant::fromValue(vectorRole[indexRow].value<QSharedPointer<LinkConfiguration>>().get());
        }

        return _sensors[role][indexRow];
    }

    /**
     * @brief Returns the model's role names.
     *
     * @return QHash<int, QByteArray>
     */
    QHash<int, QByteArray> roleNames() const override
    {
        return _roleNames;
    }

    /**
     * @brief Returns the number of rows under the given parent
     *
     * @param parent
     * @return Q_INVOKABLE rowCount
     */
    Q_INVOKABLE int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return _sensors[Available].size();
    }
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

    /**
     * @brief Return primary sensor object inside a QVariant
     * This should be QVariant since the sensor can be different type of pointers
     *
     * @return QVariant
     */
    QVariant primarySensor() const
    {
        return QVariant::fromValue(_primarySensor.get());
    }
    Q_PROPERTY(QVariant primarySensor READ primarySensor NOTIFY primarySensorChanged)

signals:
    void countChanged();
    void sensorChanged(int objIndex);
    void primarySensorChanged();

private:
    Q_DISABLE_COPY(DeviceManager)

    /**
     * @brief Construct a new Device Manager object
     *
     */
    DeviceManager();

    /**
     * @brief Update list of available links
     *
     * @param availableLinkConfigurations
     */
    void updateAvailableConnections(const QVector<LinkConfiguration>& availableLinkConfigurations);

    // Role and names
    enum Roles {
        Available = 0,
        Connected,
        Connection,
        Name,
    };
    QHash<int, QByteArray> _roleNames {
        {{Available}, {"available"}},
        {{Connected}, {"connected"}},
        {{Connection}, {"connection"}},
        {{Name}, {"name"}},
    };

    QSharedPointer<Sensor> _primarySensor;
    ProtocolDetector* _detector;
    Ping360EthernetFinder _ping360EthernetFinder;
    QThread _detectorThread;

    // Model variables
    QVector<int> _roles;
    QHash<int, QVector<QVariant>> _sensors;
};
