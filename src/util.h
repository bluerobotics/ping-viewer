#pragma once

#include <QLoggingCategory>
#include <QtCharts/QAbstractSeries>

QT_CHARTS_USE_NAMESPACE

Q_DECLARE_LOGGING_CATEGORY(util);

class QQuickView;

class Util : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE void update(QAbstractSeries* series, const QList<double>& points, const float multiplier = 1.0f);
    Q_INVOKABLE QStringList serialPortList();

    ~Util();
    static Util* self();

private:
    Util* operator = (Util& other) = delete;
    Util(const Util& other) = delete;
    Util();
};
