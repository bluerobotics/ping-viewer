#pragma once

#include <QTimer>
#include <QUdpSocket>

#include "linkconfiguration.h"

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
class Ping360EthernetFinder : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new Ping360 Ethernet Finder object
     *
     */
    Ping360EthernetFinder();

    /**
     * @brief Starts broadcasting the discovery message every second
     *
     */
    void start();

    /**
     * @brief Broadcasts one UDP "Discovery" packet
     *
     */
    void doBroadcast();

    /**
     * @brief Stops broadcasting the discovery message
     *
     */
    void stop();

signals:

    void availableLinkFound(const QVector<LinkConfiguration>& availableLinkConfigurations, const QString& detector);

private slots:

    /**
     * @brief Process responses to UDP discovery broadcast and appends found devices
     *
     */
    void processBroadcastResponses();

private:
    Q_DISABLE_COPY(Ping360EthernetFinder)

    QUdpSocket _broadcastSocket;
    QTimer _broadcastTimer;
};
