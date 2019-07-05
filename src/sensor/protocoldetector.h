#pragma once

#include <QThread>

#include "abstractlink.h"
#include "linkconfiguration.h"
#include "parser-ping.h"

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
    void appendConfiguration(const LinkConfiguration& linkConfig)
    {
        if(_linkConfigs.contains(linkConfig)) {
            return;
        }
        _linkConfigs.prepend(linkConfig);
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

signals:
    void availableLinksChanged(QVector<LinkConfiguration> linkConfs);
    void connectionDetected(LinkConfiguration linkConf);
    void scan();

protected:
    bool canOpenPort(QSerialPortInfo& port, int msTimeout);
    bool checkBuffer(const QByteArray& buffer, LinkConfiguration& linkConf);
    bool checkSerial(LinkConfiguration& linkConf);

    /**
     * @brief Check if a device is provided by a UDP server
     *  This functions follows IBM documentation about socket connections for clients
     *  https://www.ibm.com/support/knowledgecenter/en/ssw_ibm_i_71/rzab6/howdosockets.htm
     *  https://www.ibm.com/support/knowledgecenter/en/SSB23S_1.1.0.14/gtpc1/gtpc1mst104.html
     *      1. create socket
     *      2. bind (optional, but necessary for QUdpSocket::ReuseAddressHint)
     *      3. read/write
     *      4. close
     *
     *  Without ReuseAddressHint, the next connection request will fail if there were connections open.
     *
     *  Since this uses the connectToHost method, all read and write functions should use the QIODevice primitive
     *
     * @param linkConf
     * @return true
     * @return false
     */
    bool checkUdp(LinkConfiguration& linkConf);
    QVector<LinkConfiguration> updateLinkConfigurations(QVector<LinkConfiguration>& linkConfig) const;

private slots:
    void doScan();

private:
    Q_DISABLE_COPY(ProtocolDetector)
    bool _active { false };
    bool _detected { false };
    QVector<LinkConfiguration> _availableLinks;
    QVector<LinkConfiguration> _linkConfigs;
    static const QStringList _invalidSerialPortNames;
    QByteArray _deviceInformationMessageByteArray;
};
