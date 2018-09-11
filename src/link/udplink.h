#pragma once

#include <QUdpSocket>

#include "abstractlink.h"

class UDPLink : public AbstractLink
{
public:
    UDPLink(QObject* parent = nullptr);
    ~UDPLink();

    bool isOpen() final { return _udpSocket->isWritable() && _udpSocket->isReadable(); };
    bool setConfiguration(const LinkConfiguration& linkConfiguration) final;
    bool startConnection() final { return _udpSocket->open(QIODevice::ReadWrite); };
    bool finishConnection() final;
    QString errorString() final { return _udpSocket->errorString(); };
    QUdpSocket* udpSocket() { return _udpSocket; };

private:
    QHostAddress _hostAddress;
    QUdpSocket* _udpSocket;
    uint _port;
};
