#pragma once
#include <QLoggingCategory>

#define MAVLINK_MESSAGE_CRCS                                                                                           \
    {                                                                                                                  \
        {                                                                                                              \
            30, 39, 28, 28, 0, 0, 0                                                                                    \
        },                                                                                                         \
    }
#include <mavlink.h>

#include "link.h"
#include "linkconfiguration.h"

class QJSEngine;
class QQmlEngine;

Q_DECLARE_LOGGING_CATEGORY(MAVLINKMANAGER)

/**
 * @brief Manage the project MavlinkManager
 *
 */
class MavlinkManager : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Connect with link configuration
     *
     */
    void connect(const LinkConfiguration& conf);

    /**
     * @brief Parse data for the mavlink message
     *
     * @param data
     */
    void parseData(const QByteArray data);

    /**
     * @brief Return MavlinkManager pointer
     *
     * @return MavlinkManager*
     */
    static MavlinkManager* self();

    /**
     * @brief Return a pointer of this singleton to the qml register function
     *
     * @param engine
     * @param scriptEngine
     * @return QObject*
     */
    static QObject* qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine);

signals:
    void mavlinkMessage(const mavlink_message_t& message) const;

private:
    Q_DISABLE_COPY(MavlinkManager)
    /**
     * @brief Construct a new Network Manager object
     *
     */
    MavlinkManager();

    /**
     * @brief Send a heartbeat message
     *
     */
    void sendHeartbeatMessage();

    QSharedPointer<Link> _linkIn;
    QTimer _heartbeatTimer;
};
