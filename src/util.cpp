#include <QSerialPortInfo>
#include <QtCharts/QXYSeries>

#include "logger.h"
#include "util.h"

PING_LOGGING_CATEGORY(util, "ping.util");

Util::Util()
{

}

QStringList Util::serialPortList()
{
    static QSerialPortInfo serialPortInfo;
    static const QList<QSerialPortInfo> serialPortInfoList{serialPortInfo.availablePorts()};
    static QStringList portNameList;
    portNameList.clear();
    for (const auto& serialPortInfo : serialPortInfoList) {
        if (!serialPortInfo.portName().startsWith(QStringLiteral("cu."), Qt::CaseInsensitive)) {
            portNameList.append(serialPortInfo.portName());
        }
    }
    return portNameList;
}

void Util::update(QAbstractSeries* series, const QList<double>& points,
                  const float multiplier, const float upDownSampling)
{
    if (!series && points.isEmpty()) {
        qCDebug(util) << "Serie or vector not valid.";
    }

    QXYSeries *xySeries = static_cast<QXYSeries *>(series);
    // Use replace instead of clear + append, it's optimized for performance
    QVector<QPointF> realPoints;
    #pragma omp for
    for(int i = 0; i < points.size(); i++) {
        realPoints << QPointF(i, multiplier * points[i]);
    }
    int finalSize = points.size()*upDownSampling;
    #pragma omp for
    for(int i = points.size(); i < finalSize; i++) {
        realPoints << QPointF(i, 0);
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
