#include <QDebug>

#include "abstractlink.h"
#include "abstractlinknamespace.h"

const QString AbstractLink::_timeFormat = QStringLiteral("hh:mm:ss.zzz");

AbstractLink::AbstractLink(const QString& name, QObject* parent)
    : QObject(parent)
    , _autoConnect(false)
    , _name(name)
    , _type(LinkType::None)
{
    connect(this, &AbstractLink::newData, this,
        [&](const QByteArray& data) { _bitRateDownSpeed.numberOfBytes += data.size(); });
    connect(this, &AbstractLink::sendData, this,
        [&](const QByteArray& data) { _bitRateUpSpeed.numberOfBytes += data.size(); });
    connect(&_oneSecondTimer, &QTimer::timeout, this, [&]() {
        _bitRateDownSpeed.update();
        _bitRateUpSpeed.update();
        emit speedChanged();
    });

    _oneSecondTimer.start(1000);
}

AbstractLink::~AbstractLink() = default;
