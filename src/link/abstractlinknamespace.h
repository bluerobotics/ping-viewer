#pragma once

#include <QObject>

namespace AbstractLinkNamespace
{
Q_NAMESPACE
enum LinkType {
    None = 0,
    File,
    Serial,
    Udp,
    Tcp,
    Ping1DSimulation,
    Last,
};
Q_ENUM_NS(LinkType)
};

using namespace AbstractLinkNamespace;