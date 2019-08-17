#include "logthread.h"

#include <QDebug>

#include "logger/logger.h"

PING_LOGGING_CATEGORY(LOGTHREAD, "ping.logthread");

LogThread::LogThread(QObject *parent)
    :QTimer(parent)
    ,_logIndex(0)
    ,_playLog(true)
{
    setSingleShot(true);
    connect(this, &QTimer::timeout, this, &LogThread::processJob);
}

void LogThread::processJob()
{
    // Check for pause condition and valid log index
    if(!_playLog) {
        return;
    }

    static int lastMSecs = 0;
    QByteArray& data = _log[_logIndex].data;
    lastMSecs = _log[_logIndex].time.msecsSinceStartOfDay();
    emit packageIndexChanged(_logIndex);
    emit newPackage(data);

    // Check if we have data before sending
    if(_logIndex < _log.size() - 1) {
        static int diffMSecs;
        _logIndex++;
        diffMSecs = _log[_logIndex].time.msecsSinceStartOfDay() - lastMSecs;

        // Something is wrong, we need to go 'back to the future'
        if(diffMSecs < 0) {
            qCWarning(LOGTHREAD) << "Sample time is negative from previous sample! Trying to recover..";
            qCDebug(LOGTHREAD) << "First time:" << _log.constFirst().time;
            qCDebug(LOGTHREAD) << "Last time:" << _log.constLast().time;
            qCDebug(LOGTHREAD) << "Actual index:" << _logIndex
                               << "Time[n-1, n]:" << _log[_logIndex - 1].time << _log[_logIndex].time;
            processJob();
            return;
        }
        start(diffMSecs/10);
    }
}

QTime LogThread::totalTime()
{
    int totalMSecs = _log.constLast().time.msecsSinceStartOfDay() - _log.constFirst().time.msecsSinceStartOfDay();
    return QTime::fromMSecsSinceStartOfDay(totalMSecs);
}

QTime LogThread::elapsedTime()
{
    if(_logIndex < 0) {
        return QTime::fromMSecsSinceStartOfDay(0);
    } else if(_logIndex >= _log.size()) {
        return totalTime();
    }

    int elapsedMSecs = _log[_logIndex].time.msecsSinceStartOfDay() - _log.constFirst().time.msecsSinceStartOfDay();
    return QTime::fromMSecsSinceStartOfDay(elapsedMSecs);
}

LogThread::~LogThread() = default;