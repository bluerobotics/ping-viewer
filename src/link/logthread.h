#pragma once

#include <QByteArray>
#include <QLoggingCategory>
#include <QTime>
#include <QTimer>
#include <QVector>

Q_DECLARE_LOGGING_CATEGORY(LOGTHREAD)

/**
 * @brief Play sensor logs
 *
 */
class LogThread : public QTimer
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new Log Thread object
     *
     * @param parent
     */
    LogThread(QObject *parent = nullptr);

    /**
     * @brief Destroy the Log Thread object
     *
     */
    ~LogThread();

    /**
     * @brief Append new log message
     *
     * @param time
     * @param data
     */
    void append(QTime time, QByteArray data) { _log.append(Pack{time, data}); }

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
    int packageIndex() const { return _logIndex; }

    /**
     * @brief Return package size
     *
     * @return int
     */
    int packageSize() const { return _log.size() - 1; }

    /**
     * @brief Pause log
     *
     */
    void pauseJob() { _playLog = false; }

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
    void startJob() { _playLog = true; start();}

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
    bool _playLog;
};
