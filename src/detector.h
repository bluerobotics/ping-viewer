#pragma once

#include <QThread>

#include "abstractlink.h"
#include "linkconfiguration.h"
#include "parsers/parser_ping.h"

class QSerialPortInfo;

/// This class will scan network ports and serial ports for a ping device
/// TODO subclass and support discovery of other protocols/devices
class ProtocolDetector : public QObject
{
    Q_OBJECT
public:
    ProtocolDetector();

    void appendConfiguration(LinkConfiguration& linkConfig)
    {
        _linkConfigs.append(linkConfig);
    }

    QVector<LinkConfiguration> availableLinks()
    {
        return _availableLinks;
    };

    bool checkLink(LinkConfiguration& linkConf);

    static const QStringList& invalidSerialPortNames()
    {
        return _invalidSerialPortNames;
    };
    bool isValidPort(const QSerialPortInfo& serialPortInfo) const;
    bool isRunning() const { return _active; };
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
    bool _active { false };
    bool _detected { false };
    QVector<LinkConfiguration> _availableLinks;
    QVector<LinkConfiguration> _linkConfigs;
    PingParser _parser;
    static const QStringList _invalidSerialPortNames;
};
