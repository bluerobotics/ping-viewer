#include <QDebug>

#include "abstractlink.h"
#include "abstractlinknamespace.h"

const QString AbstractLink::_timeFormat = QStringLiteral("hh:mm:ss.zzz");

AbstractLink::AbstractLink(QObject* parent)
    : QObject(parent)
    , _type(LinkType::None)
{
}

AbstractLink::~AbstractLink() = default;
