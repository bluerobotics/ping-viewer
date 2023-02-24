#include "ping360asciiprotocol.h"

#include <QHostAddress>

QByteArray Ping360AsciiProtocol::discoveryMessage() { return {"Discovery"}; }

Ping360DiscoveryResponse Ping360AsciiProtocol::decodeDiscoveryResponse(const QString& response)
{
    auto lines = response.split(QStringLiteral("\r\n"));
    QString ipAddress;

    for (auto& line : lines) {
        line = line.trimmed();
        if (line.startsWith("IP Address:- ")) {
            ipAddress = line.remove("IP Address:- ");
            line = "IP Address:- " + Ping360AsciiProtocol::removeLeadingZerosFromIP(ipAddress);
        }
    };

    if (lines.size() == Ping360DiscoveryResponse::numberOfLines) {
        QStringList splitted = lines[3].split(":- ");
        if (splitted.size() < 2) {
            qWarning() << "Invalid format in line[3], ':- ' not found.";
            return {};
        }
        return {lines[0], lines[1], lines[2], splitted[1]};
    }

    return {};
}

QString Ping360AsciiProtocol::removeLeadingZerosFromIP(const QString& line)
{
    QStringList ipParts = line.split(".");
    for (int i = 0; i < ipParts.size(); i++) {
        ipParts[i] = QString::number(ipParts[i].toInt());
    }
    return ipParts.join(".");
}

QByteArray Ping360AsciiProtocol::staticIpAddressMessage(const QString& address)
{
    union Ipv4Union {
        uint8_t values[4];
        uint32_t ip;

        // Return value with xxx format with leading zeros if necessary
        QString getPing360IpValueFromIndex(int index) const
        {
            return QString::number(values[index]).rightJustified(3, '0');
        }
    } ipv4Union;

    QHostAddress sensorAddress(address);
    bool ok = false;

    // Get IPV4 value from input, and return a DHCP client configuration if an invalid address is received
    ipv4Union.ip = sensorAddress.toIPv4Address(&ok);
    if (!ok) {
        ipv4Union.ip = 0;
    }

    return QStringLiteral("SetSS1IP %1.%2.%3.%4")
        .arg(ipv4Union.getPing360IpValueFromIndex(3), ipv4Union.getPing360IpValueFromIndex(2),
            ipv4Union.getPing360IpValueFromIndex(1), ipv4Union.getPing360IpValueFromIndex(0))
        .toLatin1();
}
