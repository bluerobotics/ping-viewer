#include "mavlinkmanager.h"

#include "logger.h"

#include <mavlink_msg_attitude.h>

#include <QDebug>
#include <QQmlEngine>

PING_LOGGING_CATEGORY(MAVLINKMANAGER, "ping.mavlinkmanager")

MavlinkManager::MavlinkManager()
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // This is our default udpin connection link
    // TODO: Allow the user to configure this
    connect({LinkType::Udp, {"192.168.2.2", "14660"}, "MAVLinkManager"});

    QObject::connect(&_heartbeatTimer, &QTimer::timeout, this, &MavlinkManager::sendHeartbeatMessage);
    _heartbeatTimer.start(4000);
}

void MavlinkManager::connect(const LinkConfiguration& conf)
{
    if (_linkIn) {
        auto link = _linkIn->self();
        if (link->isOpen()) {
            link->finishConnection();
        }
    }

    qCDebug(MAVLINKMANAGER) << "Connecting to" << conf;
    if (!conf.isValid()) {
        qCWarning(MAVLINKMANAGER) << LinkConfiguration::errorToString(conf.error());
        return;
    }
    if (_linkIn) {
        _linkIn.clear();
    }
    _linkIn = QSharedPointer<Link>(new Link(conf));
    _linkIn->self()->startConnection();

    QObject::connect(_linkIn->self(), &AbstractLink::newData, this, &MavlinkManager::parseData);
}

void MavlinkManager::parseData(const QByteArray data)
{
    static mavlink_message_t message;
    mavlink_status_t status;
    for (const auto& byte : data) {
        if (mavlink_parse_char(0, byte, &message, &status)) {
            emit mavlinkMessage(message);
        }
    }
}

void MavlinkManager::sendHeartbeatMessage() { _linkIn->self()->write(_heartbeatMessage); }

QObject* MavlinkManager::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}

MavlinkManager* MavlinkManager::self()
{
    static MavlinkManager self;
    return &self;
}
