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

    /**
     * @brief Create a QAbstractSeries from a list of points
     *
     * @param series
     * @param points
     * @param multiplier This is a scalar to be used when creating the serie
     */
    Q_INVOKABLE void update(QAbstractSeries* series, const QList<double>& points, const float multiplier = 1.0f);

    /**
     * @brief Return a list of the available serial ports
     *
     * @return QStringList serialPortList
     */
    Q_INVOKABLE QStringList serialPortList();

    ~Util();
    static Util* self();

private:
    Util* operator = (Util& other) = delete;
    Util(const Util& other) = delete;
    Util();
};
