#include <QDateTime>
#include <QDebug>
#include <QLoggingCategory>

#include "filelink.h"
#include "filemanager.h"
#include "sensor.h"

#include <ping-message-common.h>
#include <ping-message.h>

PING_LOGGING_CATEGORY(PING_PROTOCOL_SENSOR, "ping.protocol.sensor")

const QUrl Sensor::_defaultControlPanelUrl("qrc:/NoControlPanel.qml");

Sensor::Sensor(SensorInfo sensorInfo)
    : _connected(false)
    , _controlPanelUrl(_defaultControlPanelUrl)
    , _flasher(nullptr)
    , _linkIn(new Link())
    , _linkOut(nullptr)
    , _parser(nullptr)
    , _sensorInfo(sensorInfo)
{
    connect(this, &Sensor::connectionOpen, this, [this] {
        _connected = true;
        emit connectionChanged();
    });
    connect(this, &Sensor::connectionClose, this, [this] {
        _connected = false;
        emit connectionChanged();
    });
}

// TODO: rework this after sublasses and parser rework
void Sensor::connectLink(const LinkConfiguration conConf, const LinkConfiguration& logConf)
{
    if (link()->isOpen()) {
        link()->finishConnection();
    }

    qCDebug(PING_PROTOCOL_SENSOR) << "Connecting to" << conConf;
    if (!conConf.isValid()) {
        qCWarning(PING_PROTOCOL_SENSOR) << LinkConfiguration::errorToString(conConf.error());
        return;
    }
    if (link()) {
        _linkIn.clear();
    }
    _linkIn = QSharedPointer<Link>(new Link(conConf));
    link()->startConnection();

    if (!link()->isOpen()) {
        qCCritical(PING_PROTOCOL_SENSOR) << "Connection fail !" << conConf << link()->errorString();
        emit connectionClose();
        return;
    }

    emit linkChanged();

    if (_parser) {
        _parser->clearBuffer();
        connect(link(), &AbstractLink::newData, _parser, &Parser::parseBuffer, Qt::DirectConnection);
    }

    emit connectionOpen();

    // Disable log if playing one
    // Only save last link configuration if it's not a log
    if (link()->type() == LinkType::File) {
        if (!linkLog()) {
            return;
        }

        if (linkLog()->isOpen()) {
            linkLog()->finishConnection();
            _linkOut.clear();
        }
    } else {
        if (!logConf.isValid()) {
            connectLinkLog({LinkType::File,
                {FileManager::self()->createFileName(FileManager::Folder::SensorLog), QStringLiteral("w")}});
        } else {
            connectLinkLog(logConf);
        }
    }
}

void Sensor::connectLinkLog(const LinkConfiguration& logConf)
{
    if (linkLog()) {
        if (!linkLog()->isOpen()) {
            qCCritical(PING_PROTOCOL_SENSOR) << "No connection to log !" << linkLog()->errorString();
            return;
        }
        _linkOut.clear();
    }

    if (!logConf.isValid()) {
        qCWarning(PING_PROTOCOL_SENSOR) << LinkConfiguration::errorToString(logConf.error());
        return;
    }

    _linkOut = QSharedPointer<Link>(new Link(logConf));

    // Configure log file
    auto fileLink = qobject_cast<FileLink*>(linkLog());
    if (fileLink) {
        fileLink->logSensorStruct()->setSensorInfo(_sensorInfo);
    }

    linkLog()->startConnection();

    if (!linkLog()->isOpen()) {
        qCCritical(PING_PROTOCOL_SENSOR) << "Connection with log fail !" << logConf << linkLog()->errorString();
        return;
    }

    connect(link(), &AbstractLink::newData, linkLog(), &AbstractLink::sendData);
    emit linkLogChanged();
}

void Sensor::setControlPanel(const QUrl& url)
{
    if (!url.isValid()) {
        qCCritical(PING_PROTOCOL_SENSOR) << "Invalid url:" << url;
        _controlPanelUrl = _defaultControlPanelUrl;
        return;
    }

    _controlPanelUrl = url;
}

void Sensor::setSensorVisualizer(const QUrl& url)
{
    if (!url.isValid()) {
        qCCritical(PING_PROTOCOL_SENSOR) << "Invalid url:" << url;
        // TODO: Need default visualizer
        _sensorVisualizerUrl.setUrl({});
        return;
    }

    _sensorVisualizerUrl = url;
}

void Sensor::setSensorStatusModel(const QUrl& url)
{
    if (!url.isValid()) {
        qCCritical(PING_PROTOCOL_SENSOR) << "Invalid url:" << url;
        _sensorStatusModelUrl.setUrl({});
        return;
    }

    _sensorStatusModelUrl = url;
}

bool Sensor::createQQuickItem(QObject* parent, const QUrl& resource, QSharedPointer<QQuickItem>& pointerQuickItem)
{
    QQmlEngine* engine = qmlEngine(parent);
    if (!engine) {
        qCCritical(PING_PROTOCOL_SENSOR) << "No qml engine to load visualization for:" << resource;
        return false;
    }
    QQmlComponent component(engine, resource, parent);
    pointerQuickItem.reset(qobject_cast<QQuickItem*>(component.create()));
    if (pointerQuickItem.isNull()) {
        qCCritical(PING_PROTOCOL_SENSOR) << "Failed to load QML component:" << resource;
        qCDebug(PING_PROTOCOL_SENSOR) << "Component status:" << component.status();
        if (component.isError()) {
            qCDebug(PING_PROTOCOL_SENSOR) << "Error list:" << component.errors();
        }
        return false;
    }

    pointerQuickItem->setParentItem(qobject_cast<QQuickItem*>(parent));
    return !pointerQuickItem.isNull();
}

QQuickItem* Sensor::controlPanel(QObject* parent)
{
    // Check if item already exist, if not create it
    if (_controlPanel.isNull() && !createQQuickItem(parent, _controlPanelUrl, _controlPanel)) {
        qCCritical(PING_PROTOCOL_SENSOR) << "Failed to create control panel.";
        return nullptr;
    }
    qDebug() << _controlPanel.get();
    return _controlPanel.get();
}

QQuickItem* Sensor::sensorVisualizer(QObject* parent)
{
    // Check if item already exist, if not create it
    if (_sensorVisualizer.isNull()) {
        if (!createQQuickItem(parent, _sensorVisualizerUrl, _sensorVisualizer)) {
            qCCritical(PING_PROTOCOL_SENSOR) << "Failed to create sensor visualizer.";
            return nullptr;
        }
        // Visualizer will always fill the parent
        auto anchors = _sensorVisualizer.get()->property("anchors").value<QObject*>();
        anchors->setProperty("fill", QVariant::fromValue(parent));
    }

    return _sensorVisualizer.get();
}

QQuickItem* Sensor::sensorStatusModel(QObject* parent)
{
    // Check if item already exist, if not create it
    if (_sensorStatusModel.isNull()) {
        if (!createQQuickItem(parent, _sensorStatusModelUrl, _sensorStatusModel)) {
            qCCritical(PING_PROTOCOL_SENSOR) << "Failed to create sensor status.";
            return nullptr;
        }
    }
    _sensorStatusModel->setProperty("sensor", QVariant::fromValue(this));

    return _sensorStatusModel.get();
}

void Sensor::resetSettings() { qCWarning(PING_PROTOCOL_SENSOR) << "No reset settings definition."; }

void Sensor::setName(const QString& name)
{
    if (_name == name) {
        return;
    }

    _name = name;
    emit nameChanged();
}

Sensor::~Sensor() = default;
