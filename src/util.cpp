#include <QtCharts/QXYSeries>

#include "logger.h"
#include "util.h"

PING_LOGGING_CATEGORY(util, "ping.util");

Util::Util()
{

}

void Util::update(QAbstractSeries* series, const QList<double>& points, const bool invert)
{
    if (!series && points.isEmpty()) {
        qCDebug(util) << "Serie or vector not valid.";
    }

    QXYSeries *xySeries = static_cast<QXYSeries *>(series);
    // Use replace instead of clear + append, it's optimized for performance
    QVector<QPointF> realPoints;
    float signal = invert ? -1 : 1;
    float mult = 255*signal;
    for(int i(0); i < points.size(); i++) {
        realPoints << QPointF(i, mult*points[i]);
    }
    xySeries->replace(realPoints);
}

Util* Util::self()
{
    static Util* self = new Util();
    return self;
}

Util::~Util()
{

}