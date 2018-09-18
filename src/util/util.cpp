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
    static QStringList portNameList;
    const QList<QSerialPortInfo> serialPortInfoList{serialPortInfo.availablePorts()};
    portNameList.clear();
    for (const auto& serialPortInfo : serialPortInfoList) {
        if (!serialPortInfo.portName().startsWith(QStringLiteral("cu."), Qt::CaseInsensitive)) {
            portNameList.append(serialPortInfo.portName());
        }
    }
    return portNameList;
}

void Util::update(QtCharts::QAbstractSeries* series, const QVector<double>& points,
                  const float initPos, const float finalPos,
                  const float minPoint, const float maxPoint,
                  const float multiplier)
{
    static const int numberOfPoints = 300;

    // Check inputs
    if (!series && points.isEmpty()) {
        qCDebug(util) << "Serie or vector not valid.";
        return;
    }

    if(initPos > finalPos) {
        qCDebug(util) << "InitPos need to be lower than finalPos.";
        return;
    }

    // Points per Meter
    const float distPoints = numberOfPoints/(maxPoint - minPoint);

    QXYSeries *xySeries = static_cast<QXYSeries *>(series);

    // Use replace instead of clear + append, it's optimized for performance
    QVector<QPointF> realPoints;
    realPoints.reserve(numberOfPoints);

    // Start
    const int lastStartPoint = int(distPoints*(initPos-minPoint));
    #pragma omp for
    for(int i = 0; i < lastStartPoint; i++) {
        realPoints << QPointF(i, 0);
    }

    // Data
    const int lastDataPoint = int((finalPos - initPos)*distPoints);
    const float dataIndexScale = points.length()/((finalPos - initPos)*distPoints);
    #pragma omp for
    for(int i = 0; i < lastDataPoint; i++) {
        realPoints << QPointF(i + lastStartPoint, multiplier * points[i*dataIndexScale]);
    }

    // Final
    #pragma omp for
    for(int i = realPoints.length(); i < numberOfPoints; i++) {
        realPoints << QPointF(i, 0);
    }

    // Do replace
    xySeries->replace(realPoints);
}

QObject* Util::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}

Util* Util::self()
{
    static Util* self = new Util();
    return self;
}

Util::~Util()
{

}
