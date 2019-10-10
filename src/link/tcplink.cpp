#include <QDebug>

#include "tcplink.h"

TCPLink::TCPLink(QObject* parent)
    : AbstractLink("TCPLink", parent)
{
}

TCPLink::~TCPLink() { finishConnection(); }
