#pragma once

#include <QLoggingCategory>
#include <QtCharts>

class QJSEngine;
class QQmlEngine;

Q_DECLARE_LOGGING_CATEGORY(util)

class QQuickView;

/**
 * @brief Singleton helper for qml interface
 *
 */
class Util : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Create a QAbstractSeries from a list of points
     *
     * @param series
     * @param points
     * @param initPos
     * @param finalPos
     * @param minPoint
     * @param maxPoint
     * @param multiplier
     */
    Q_INVOKABLE void update(QtCharts::QAbstractSeries* series, const QVector<double>& points, const float initPos,
        const float finalPos, const float minPoint, const float maxPoint, const float multiplier = 1);

    /**
     * @brief Return a list of the available serial ports
     *
     * @return QStringList serialPortList
     */
    Q_INVOKABLE QStringList serialPortList();

    /**
     * @brief Check if the application is running under macos
     *
     * @return bool
     */
    Q_INVOKABLE bool isMac() const
    {
        return QSysInfo::prettyProductName().contains(QStringLiteral("macos"), Qt::CaseInsensitive);
    }

    /**
     * @brief  Restart application
     *  Based in https://stackoverflow.com/a/7947551/7988054
     */
    Q_INVOKABLE void restartApplication();

    /**
     * @brief Return Util pointer
     *
     * @return Util*
     */
    static Util* self();
    ~Util();

    /**
     * @brief Return a pointer of this singleton to the qml register function
     *
     * @param engine
     * @param scriptEngine
     * @return QObject*
     */
    static QObject* qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine);

private:
    Q_DISABLE_COPY(Util)
    /**
     * @brief Construct a new Util object
     *
     */
    Util() = default;
};
