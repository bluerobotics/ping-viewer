#pragma once

#include <QLoggingCategory>
#include <QTimer>
#include <QUdpSocket>

#include "linkconfiguration.h"

Q_DECLARE_LOGGING_CATEGORY(PING360HELPERSERVICE)

class QJSEngine;
class QQmlEngine;

/**
 * @brief This class will scan the network for Ping360 devices
 * This is implemented following the "Communications Manual 1.7".
 *
 * Every second we do a broadcast for the ASCII message "Discovery" to UDP
 * port 30303 and IP 255.255.255.255. The devices responds back at the sending
 * port with a message with this format:
 *
 *          SONAR PING360
            Blue Robotics
            MAC Address:- DE-AD-BE-EF-12-34
            IP Address:- 192.168.012.034

    When such a message is received, we emit a availableLinkFound() with the
    found device.
 */
class Ping360HelperService : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Return Ping360HelperService pointer
     *
     * @return Ping360HelperService*
     */
    static Ping360HelperService* self();

    /**
     * @brief Return a pointer of this singleton to the qml register function
     *
     * @param engine
     * @param scriptEngine
     * @return QObject*
     */
    static QObject* qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine);

    /**
     * @brief Starts broadcasting the discovery message every second
     *
     */
    void startBroadcastService();

    /**
     * @brief Broadcasts one UDP "Discovery" packet
     *
     */
    void doBroadcast();

    /**
     * @brief Stops broadcasting the discovery message
     *
     */
    void stopBroadcastService();

    /**
     * @brief Configure device under specific IP as DHCP client
     *
     * @param ip
     */
    Q_INVOKABLE void setDHCPServer(const QString& ip);

    /**
     * @brief Configure device under specific IP as static IP client
     *
     * @param ip
     * @param staticIp
     */
    Q_INVOKABLE void setStaticIP(const QString& ip, const QString& staticIp);

signals:
    void availableLinkFound(const QVector<LinkConfiguration>& availableLinkConfigurations, const QString& detector);

private slots:

    /**
     * @brief Process responses to UDP discovery broadcast and appends found devices
     *
     */
    void processBroadcastResponses();

private:
    Q_DISABLE_COPY(Ping360HelperService)

    /**
     * @brief Construct a new Ping360 Helper Service
     *
     */
    Ping360HelperService();

    QUdpSocket _broadcastSocket;
    QTimer _broadcastTimer;
};
