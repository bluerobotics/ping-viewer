#include <QDebug>

#include "abstractlink.h"
#include "abstractlinknamespace.h"

const QString AbstractLink::_timeFormat = QStringLiteral("hh:mm:ss.zzz");

AbstractLink::AbstractLink(QObject* parent)
    : QObject(parent)
    , _type(LinkType::None)
{
}

const AbstractLink& AbstractLink::operator=(const AbstractLink& other)
{
    _autoConnect = other._autoConnect;
    _type = other._type;
    _name = other._name;
    return *this;
}

AbstractLink::~AbstractLink()
{
    finishConnection();
}