#include <QDebug>
#include <QLoggingCategory>

#include "link.h"

#include "filelink.h"
#include "ping1dsimulationlink.h"
#include "ping360simulationlink.h"
#include "seriallink.h"
#include "simulationlink.h"
#include "tcplink.h"
#include "udplink.h"

Q_LOGGING_CATEGORY(PING_PROTOCOL_LINK, "ping.protocol.link")

Link::Link(LinkType linkType, QString name, QObject* parent)
    : QObject(parent)
    , _abstractLink(nullptr)
{
    switch(linkType) {
    case LinkType::None :
        _abstractLink = new AbstractLink(this);
        break;
    case LinkType::File :
        _abstractLink = new FileLink(this);
        break;
    case LinkType::Serial :
        _abstractLink = new SerialLink(this);
        break;
    case LinkType::Udp :
        _abstractLink = new UDPLink(this);
        break;
    case LinkType::Ping1DSimulation :
        _abstractLink = new Ping1DSimulationLink(this);
        break;
    case LinkType::Ping360Simulation :
        _abstractLink = new Ping360SimulationLink(this);
        break;
    default :
        assert("Link not available!");
        return;
    }

    _abstractLink->setName(name);
}

Link::Link(const LinkConfiguration& linkConfiguration, QObject* parent)
    : QObject(parent)
    , _abstractLink(nullptr)
{
    switch(linkConfiguration.type()) {
    case LinkType::File :
        _abstractLink = new FileLink(this);
        break;
    case LinkType::Serial :
        _abstractLink = new SerialLink(this);
        break;
    case LinkType::Udp :
        _abstractLink = new UDPLink(this);
        break;
    case LinkType::Ping1DSimulation :
        _abstractLink = new Ping1DSimulationLink(this);
        break;
    case LinkType::Ping360Simulation :
        _abstractLink = new Ping360SimulationLink(this);
        break;
    default :
        assert("Link not available!");
        return;
    }

    _abstractLink->setConfiguration(linkConfiguration);
}
