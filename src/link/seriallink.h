#pragma once

#include <QSerialPort>

#include "abstractlink.h"

class SerialLink : public AbstractLink
{

public:
    SerialLink(QObject* parent = nullptr);
    ~SerialLink();

    bool isOpen() final { return _port->isWritable() && _port->isReadable(); };
    bool setConfiguration(const LinkConfiguration& linkConfiguration) final;
    bool startConnection() final { return _port->open(QIODevice::ReadWrite); };
    bool finishConnection() final;
    QString errorString() final { return _port->errorString(); };
    QStringList listAvailableConnections() final;
    QSerialPort* port() { return _port; };

private:
    QSerialPort* _port;
};