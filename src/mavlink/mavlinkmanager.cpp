#include "mavlinkmanager.h"

#include "logger.h"

#include <mavlink_msg_attitude.h>

#include <QDebug>
#include <QQmlEngine>

PING_LOGGING_CATEGORY(MAVLINKMANAGER, "ping.mavlinkmanager")

MavlinkManager::MavlinkManager()
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

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
    // qDebug() << "length:" << data.length();
    for (const auto& byte : data) {
        if (mavlink_parse_char(0, byte, &message, &status)) {
            qDebug() << "ID:" << message.msgid;
            emit mavlinkMessage(message);
        }
        // qDebug() << status.parse_state;
    }
}

void MavlinkManager::sendHeartbeatMessage()
{
    static mavlink_message_t message;
    static unsigned char bufffer[1024];
    mavlink_msg_heartbeat_pack(
        -1, 0, &message, MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID, MAV_MODE_FLAG_SAFETY_ARMED, 0, MAV_STATE_ACTIVE);
    auto length = mavlink_msg_to_send_buffer(bufffer, &message);
    _linkIn->self()->write(reinterpret_cast<const char*>(bufffer), length);
}

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
