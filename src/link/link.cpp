#include <QDebug>
#include <QLoggingCategory>

#include "link.h"
#include "logger.h"
#include "filelink.h"
#include "ping1dsimulationlink.h"
#include "ping360simulationlink.h"
#include "seriallink.h"
#include "simulationlink.h"
#include "tcplink.h"
#include "udplink.h"

PING_LOGGING_CATEGORY(PING_PROTOCOL_LINK, "ping.protocol.link")

Link::Link(LinkType linkType, QString name, QObject* parent)
    : QObject(parent)
    , _abstractLink(nullptr)
{
    switch(linkType) {
    case LinkType::None :
        _abstractLink.reset(new AbstractLink());
        break;
    case LinkType::File :
        _abstractLink.reset(new FileLink());
        break;
    case LinkType::Serial :
        _abstractLink.reset(new SerialLink());
        break;
    case LinkType::Udp :
        _abstractLink.reset(new UDPLink());
        break;
    case LinkType::Ping1DSimulation :
        _abstractLink.reset(new Ping1DSimulationLink());
        break;
    case LinkType::Ping360Simulation :
        _abstractLink.reset(new Ping360SimulationLink());
        break;
    default :
        qCDebug(PING_PROTOCOL_LINK) << "Link not available!";
        return;
    }

    _abstractLink->setParent(this);
    _abstractLink->setName(name);
}

Link::Link(const LinkConfiguration& linkConfiguration, QObject* parent)
    : QObject(parent)
    , _abstractLink(nullptr)
{
    switch(linkConfiguration.type()) {
    case LinkType::File :
        _abstractLink.reset(new FileLink());
        break;
    case LinkType::Serial :
        _abstractLink.reset(new SerialLink());
        break;
    case LinkType::Udp :
        _abstractLink.reset(new UDPLink());
        break;
    case LinkType::Ping1DSimulation :
        _abstractLink.reset(new Ping1DSimulationLink());
        break;
    case LinkType::Ping360Simulation :
        _abstractLink.reset(new Ping360SimulationLink());
        break;
    default :
        qCDebug(PING_PROTOCOL_LINK) << "Link not available!";
        return;
    }

    _abstractLink->setParent(this);
    _abstractLink->setConfiguration(linkConfiguration);
}

Link::~Link() = default;
