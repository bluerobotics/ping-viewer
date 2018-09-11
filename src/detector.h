#pragma once

#include <QThread>

#include "abstractlink.h"
#include "parsers/parser_ping.h"

class QSerialPortInfo;

/// This class will scan network ports and serial ports for a ping device
/// TODO subclass and support discovery of other protocols/devices
class ProtocolDetector : public QThread
{
    Q_OBJECT
public:
    ProtocolDetector()
    {
        connect(this, &QThread::finished, this, [this] { _active = false; });
    };

    static const QStringList& invalidSerialPortNames()
    {
        return _invalidSerialPortNames;
    };
    bool isValidPort(QSerialPortInfo& serialPortInfo);
    void scan();

signals:
    void connectionDetected(AbstractLinkNamespace::LinkType connType, QStringList config); // Todo can we send a pre-configured, pre-connected link object with the signal?

protected:
    void run() { scan(); }
    bool canOpenPort(QSerialPortInfo& port, int msTimeout);

private:
    bool _active { false };
    PingParser _parser;
    static const QStringList _invalidSerialPortNames;
};
