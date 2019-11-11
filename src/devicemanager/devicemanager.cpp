#include <QQmlEngine>

#include "devicemanager.h"
#include "filelink.h"
#include "logger.h"
#include "ping.h"
#include "ping360.h"

PING_LOGGING_CATEGORY(DEVICEMANAGER, "ping.devicemanager");

DeviceManager::DeviceManager()
    : _detector(new ProtocolDetector())
{
    for (const auto& key : _roleNames.keys()) {
        _roles.append(key);
        _sensors.insert(key, {});
    }

    _detector->moveToThread(&_detectorThread);
    connect(&_detectorThread, &QThread::started, _detector, &ProtocolDetector::scan);
    // We need to take care here to not erase configurations that are already connected
    // Also we should remove or set Connected to false if device is disconnected
    connect(_detector, &ProtocolDetector::availableLinksChanged, this, &DeviceManager::updateAvailableConnections);
    connect(&_ping360EthernetFinder, &Ping360EthernetFinder::availableLinkFound, this,
        &DeviceManager::updateAvailableConnections);
}

void DeviceManager::append(const LinkConfiguration& linkConf, const QString& deviceName, const QString& detectorName)
{
    for (int i {0}; i < _sensors[Connection].size(); i++) {
        auto vectorLinkConf = _sensors[Connection][i].value<QSharedPointer<LinkConfiguration>>().get();
        if (*vectorLinkConf == linkConf) {
            qCDebug(DEVICEMANAGER) << "Connection configuration already exist for:" << _sensors[Name][i] << linkConf
                                   << linkConf.argsAsConst();
            _sensors[Available][i] = true;
            const auto indexRow = index(i);
            emit dataChanged(indexRow, indexRow, _roles);
            return;
        }
    }

    qCDebug(DEVICEMANAGER) << "Add connection configuration for:" << deviceName << linkConf;

    const int line = rowCount();
    beginInsertRows(QModelIndex(), line, line);
    _sensors[Available].append(true);
    _sensors[Connection].append(
        QVariant::fromValue(QSharedPointer<LinkConfiguration>(new LinkConfiguration(linkConf))));
    _sensors[Connected].append(false);
    _sensors[DetectorName].append(detectorName);
    _sensors[Name].append(deviceName);

    const auto& indexRow = index(line);
    endInsertRows();
    emit dataChanged(indexRow, indexRow, _roles);
    emit countChanged();
}

void DeviceManager::startDetecting()
{
    qCDebug(DEVICEMANAGER) << "Start protocol detector service.";
    _detectorThread.start();
    _ping360EthernetFinder.start();
}

void DeviceManager::stopDetecting()
{
    qCDebug(DEVICEMANAGER) << "Stop protocol detector service.";
    _ping360EthernetFinder.stop();
    _detector->stop();
    _detectorThread.quit();
}

void DeviceManager::connectLink(LinkConfiguration* linkConf)
{
    // Stop detector if we are going to connect with something
    stopDetecting();

    // Find configuration in vector with valid index
    int objIndex = -1;
    for (int i {0}; i < _sensors[Connection].size(); i++) {
        auto sensorLinkConf = _sensors[Connection][i].value<QSharedPointer<LinkConfiguration>>().get();
        if (*sensorLinkConf == *linkConf) {
            objIndex = i;
            break;
        }
    }

    // index is -1 when connection configuration does not exist in list
    if (objIndex < 0) {
        qCWarning(DEVICEMANAGER) << "Connection configuration does not exist in list.";
        return;
    }

    _sensors[Name][objIndex] = PingHelper::nameFromDeviceType(linkConf->deviceType());
    qCDebug(DEVICEMANAGER) << "Connecting with sensor:" << _sensors[Name][objIndex].toString() << *linkConf;

    // We could use a single Ping instance, but since we are going to support multiple devices
    // this pointer will hold everything for us
    if (linkConf->deviceType() == PingDeviceType::PING1D) {
        _primarySensor.reset(new Ping());
    } else {
        _primarySensor.reset(new Ping360());
    }

    emit primarySensorChanged();
    _primarySensor->connectLink(*linkConf);
    _sensors[Connected][objIndex] = true;
}

void DeviceManager::connectLinkDirectly(AbstractLinkNamespace::LinkType connType, const QStringList& connString,
    PingEnumNamespace::PingDeviceType deviceType)
{
    auto linkConfiguration = LinkConfiguration {connType, connString};
    linkConfiguration.setDeviceType(deviceType);

    // Append configuration as device of type "None"
    // This will create and populate all necessary roles before connecting
    append(linkConfiguration);
    connectLink(&linkConfiguration);
}

void DeviceManager::playLogFile(AbstractLinkNamespace::LinkType connType, const QStringList& connString)
{
    // Update link configuration with sensor information that is included in the log file
    auto linkConfiguration = LinkConfiguration {connType, connString};
    linkConfiguration.setDeviceType(FileLink::staticLogSensorStruct(linkConfiguration).sensor.type.ping);

    // Append configuration as device of type "None"
    // This will create and populate all necessary roles before connecting
    append(linkConfiguration);
    connectLink(&linkConfiguration);
}

void DeviceManager::updateAvailableConnections(
    const QVector<LinkConfiguration>& availableLinkConfigurations, const QString& detector)
{
    qCDebug(DEVICEMANAGER) << "Available devices:" << availableLinkConfigurations;
    // Make all connections unavailable by default
    for (int i {0}; i < _sensors[Available].size(); i++) {
        auto linkConf = _sensors[Connection][i].value<QSharedPointer<LinkConfiguration>>();
        // TODO: rework this check, check linkconfiguration capabilities or add new ones for this case
        if (linkConf->type() == AbstractLinkNamespace::Ping1DSimulation
            || linkConf->type() == AbstractLinkNamespace::Ping360Simulation || _sensors[DetectorName][i] != detector) {
            continue;
        }
        _sensors[Available][i] = false;
        const auto indexRow = index(i);
        emit dataChanged(indexRow, indexRow, _roles);
    }

    for (const auto& link : availableLinkConfigurations) {
        append(link, PingHelper::nameFromDeviceType(link.deviceType()), detector);
    }
}

QObject* DeviceManager::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}

DeviceManager* DeviceManager::self()
{
    // The QML engine will kill it for us since this object is created inside the QML engine
    // and has as childrens QML elements
    static DeviceManager* self = new DeviceManager();
    return self;
}

DeviceManager::~DeviceManager()
{
    stopDetecting();
    _detectorThread.wait();
}
