#pragma once

#include <QThread>

#include "abstractlink.h"
#include "linkconfiguration.h"
#include "parsers/parser_ping.h"

class QSerialPortInfo;

/**
 * @brief This class will scan network ports and serial ports for a ping device
 *  TODO: Use this as a abstract class to support multiple protocols
 *
 */
class ProtocolDetector : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new Protocol Detector object
     *
     */
    ProtocolDetector();

    /**
     * @brief Append more configurations to check
     *
     * @param linkConfig
     */
    void appendConfiguration(LinkConfiguration& linkConfig)
    {
        _linkConfigs.append(linkConfig);
    }

    /**
     * @brief Return a list of all available connections configurations
     *
     * @return QVector<LinkConfiguration>
     */
    QVector<LinkConfiguration> availableLinks()
    {
        return _availableLinks;
    };

    /**
     * @brief Check if something is detected in this configuration
     *
     * @param linkConf
     * @return true
     * @return false
     */
    bool checkLink(LinkConfiguration& linkConf);

    /**
     * @brief Return a list of invalid serial devices
     *
     * @return const QStringList&
     */
    static const QStringList& invalidSerialPortNames()
    {
        return _invalidSerialPortNames;
    };

    /**
     * @brief Check if serial port is valid
     *
     * @param serialPortInfo
     * @return true
     * @return false
     */
    bool isValidPort(const QSerialPortInfo& serialPortInfo) const;

    /**
     * @brief Check detector status if is already running
     *
     * @return true
     * @return false
     */
    bool isRunning() const { return _active; };

    /**
     * @brief Stop detection loop
     *
     */
    void stop() { _active = false; };

public slots:
    void scan();

signals:
    void connectionDetected(LinkConfiguration linkConf);

protected:
    bool canOpenPort(QSerialPortInfo& port, int msTimeout);
    bool checkSerial(LinkConfiguration& linkConf);
    bool checkUdp(LinkConfiguration& linkConf);
    QVector<LinkConfiguration> updateLinkConfigurations(QVector<LinkConfiguration>& linkConfig) const;

private:
    Q_DISABLE_COPY(ProtocolDetector)
    bool _active { false };
    bool _detected { false };
    QVector<LinkConfiguration> _availableLinks;
    QVector<LinkConfiguration> _linkConfigs;
    PingParser _parser;
    static const QStringList _invalidSerialPortNames;
};
