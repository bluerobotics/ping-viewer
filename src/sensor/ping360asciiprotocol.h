#pragma once

#include <QObject>

/**
 * @brief Helper struct for discovery message response
 * It is defined by the format:
 * Sensor Name\\r\\n
 * Organization\\r\\n
 * MAC Address:- 00-00-00-00-00-00\\r\\n
 * IP address:- 127.000.000.001\\r\\n
 *
 * E.g: "SONAR PING360  \r\nBlue Robotics\r\nMAC Address:- 54-10-EC-79-7D-C5\r\nIP Address:- 192.168.000.111\r\n"
 *
 */
struct Ping360DiscoveryResponse {
    QString deviceName;
    QString organization;
    QString macAddress;
    QString ipAddress;

    static const int numberOfLines = 5;
};

/**
 * @brief Helper class that provides an abstraction over the human friendly protocol of Ping360
 * This is implemented following the "Ping360 Sonar Blue Robotics Communications Protocol v2.0" PDF.
 *
 */
class Ping360AsciiProtocol : public QObject {
    Q_OBJECT
public:
    /**
     * @brief The Discovery message used by the host computer to find Ping360.
     *
     * @return QByteArray
     */
    static QByteArray discoveryMessage();

    /**
     * @brief Decode response from discovery message
     *  Check *Ping360DiscoveryResponse* definition for more information
     *
     * @param response
     * @return Ping360DiscoveryResponse
     */
    static Ping360DiscoveryResponse decodeDiscoveryResponse(const QString& response);

    /**
     * @brief Get UDP port for this communication protocol
     *
     * @return constexpr int
     */
    static constexpr int udpPort() { return 30303; };

    /**
     * @brief Create the StaticIpAddress message for Ping360.
     * The Set IP Address message is sent by the computer to the sonar as a broadcast
     * message to IP address 255.255.255.255 using port 30303. The sonar responds with a
     * Discovery Response message.
     * By default a new sonar that has been freshly programmed will start with DHCP
     * enabled and will attempt to obtain a leased IP address from a server such as a router
     * on the network. If there is no server on the network, i.e., the sonar is directly
     * connected to a PC then the sonar will try and serve an automatic IP address to the PC
     * using the 169.254.nnn.nnn address range.
     * The Set IP Address command can be used to modify this behaviour by forcing the
     * sonar to stay at a specific IP address. The IP address is stored by the sonar
     * in nonvolatile memory and so once set the sonar will stay at this fixed IP address and will no
     * longer use DHCP to request an address from a server. To remove the fixed IP address
     * and re-enable the default DHCP/AutoIP mode it is necessary to send the Set IP
     * Address command with an address of 0.0.0.0
     * Note that if the reset IP address of 0.0.0.0 is sent then the sonar will not respond with
     * any message as it forces the sonar to execute a warm boot to re-enable DHCP/AutoIP.
     * To verify the IP address has been reset it would be necessary to wait for the sonar to
     * re-boot for at least 5 seconds and then send a Discovery Message.
     *
     * @param address Used to set the device address
     * @return QByteArray
     */
    static QByteArray staticIpAddressMessage(const QString& address);

private:
    Q_DISABLE_COPY(Ping360AsciiProtocol)

    Ping360AsciiProtocol() = delete;
};
