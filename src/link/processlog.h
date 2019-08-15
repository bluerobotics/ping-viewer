#pragma once

#include <QByteArray>
#include <QLoggingCategory>
#include <QThread>
#include <QTime>
#include <QVector>

Q_DECLARE_LOGGING_CATEGORY(PING_PROCESSLOG)

/**
 * @brief Play sensor logs
 *
 */
class ProcessLog : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new Log Thread object
     *
     * @param parent
     */
    ProcessLog(QObject *parent = nullptr);

    /**
     * @brief Destroy the Log Thread object
     *
     */
    ~ProcessLog();

    /**
     * @brief Append new log message
     *
     * @param time
     * @param data
     */
    void append(QTime time, QByteArray data) { _log.append(Pack{time, data}); };

    /**
     * @brief Return log elapsed time
     *
     * @return QTime
     */
    QTime elapsedTime();

    /**
     * @brief Return last package index
     *
     * @return int
     */
    int packageIndex() { return _logIndex; }

    /**
     * @brief Return package size
     *
     * @return int
     */
    int packageSize() { return _log.size() - 1; };

    /**
     * @brief Pause log
     *
     */
    void pause() { _play = false; };

    /**
     * @brief Stop log
     *
     */
    void stop() { _stop = true; };

    /**
     * @brief Set the package index
     *
     * @param index
     */
    void setPackageIndex(int index) { if(_logIndex < _log.size()) _logIndex = index; }

    /**
     * @brief Start playing log
     *
     */
    void start() { _play = true; _stop = false; };

    /**
     * @brief Run the main process
     *  This function runs in the thread and can be stopped by the stop() call.
     *  The function needs a thread to allow a better time response to answer as a sensor for the user,
     *  this same behaviour is not possible via QTimers since it depends of the main thread to do the signal-slot call
     *  and it has a resolution of 1ms.
     */
    void run();

    /**
     * @brief Return total time of log
     *
     * @return QTime
     */
    QTime totalTime();

signals:
    void newPackage(const QByteArray& data);
    void packageIndexChanged(int index);

private:
    void processJob();

    struct Pack {
        QTime time;
        QByteArray data;
    };

    QVector<Pack> _log;
    int _logIndex;
    bool _play;
    bool _stop;
};
