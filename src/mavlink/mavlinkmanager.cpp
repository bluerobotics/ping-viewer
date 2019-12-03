#include "mavlinkmanager.h"

#include "logger.h"

#include <QDebug>
#include <QQmlEngine>

PING_LOGGING_CATEGORY(MAVLINKMANAGER, "ping.mavlinkmanager")

MavlinkManager::MavlinkManager() { QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership); }

void MavlinkManager::connect(const LinkConfiguration& conf)
{
    if(_linkIn) {
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

    auto timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, this, [this](){
        mavlink_message_t message;
        mavlink_msg_heartbeat_pack(1, 200, &message, MAV_TYPE_GCS, MAV_AUTOPILOT_INVALID, MAV_MODE_FLAG_SAFETY_ARMED, 0, MAV_STATE_ACTIVE);
        unsigned char buf[2048];
        auto len = mavlink_msg_to_send_buffer(buf, &message);
        //qDebug() << buf;
        _linkIn->self()->write(reinterpret_cast<const char*>(buf), len);
    });
    timer->start(4000);
}

void MavlinkManager::parseData(const QByteArray& data)
{
    mavlink_message_t message;
    mavlink_status_t status;
    qDebug() << data;
    for(const auto& byte : data) {
        if(mavlink_parse_char(0, byte, &message, &status)) {
            qDebug() << message.msgid;
            emit mavlinkMessage(message);
        }
        qDebug() << status.parse_state;
    }
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
