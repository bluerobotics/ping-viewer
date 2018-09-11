#include <QDebug>

#include "tcplink.h"

TCPLink::TCPLink(QObject* parent)
    : AbstractLink(parent)
{
}

TCPLink::~TCPLink() = default;