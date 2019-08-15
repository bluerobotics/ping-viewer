#include <QDebug>

#include "logger.h"
#include "processlog.h"

PING_LOGGING_CATEGORY(PING_PROCESSLOG, "ping.ProcessLog");

ProcessLog::ProcessLog(QObject *parent)
    :QObject(parent)
    ,_logIndex(0)
    ,_play(true)
    ,_stop(false)
{
}

void ProcessLog::run()
{
    // Check for stop condition
    if(_stop) {
        return;
    }

    // Check for pause condition and valid log index
    if(!_play) {
        QThread::msleep(200);
        run();
        return;
    }

    static int lastMSecs = 0;
    const QByteArray& data = _log[_logIndex].data;
    lastMSecs = _log[_logIndex].time.msecsSinceStartOfDay();
    emit packageIndexChanged(_logIndex);
    emit newPackage(data);

    // Check if we have data before sending
    if(_logIndex >=  _log.size()) {
        qCDebug(PING_PROCESSLOG) << "End of the log.";
        stop();
    }

    static int diffMSecs;
    _logIndex++;
    diffMSecs = _log[_logIndex].time.msecsSinceStartOfDay() - lastMSecs;

    // Something is wrong, we need to go 'back to the future'
    if(diffMSecs < 0) {
        qCWarning(PING_PROCESSLOG) << "Sample time is negative from previous sample! Trying to recover..";
        qCDebug(PING_PROCESSLOG) << "First time:" << _log.constFirst().time;
        qCDebug(PING_PROCESSLOG) << "Last time:" << _log.constLast().time;
        qCDebug(PING_PROCESSLOG) << "Actual index:" << _logIndex
                                 << "Time[n-1, n]:" << _log[_logIndex - 1].time << _log[_logIndex].time;
        run();
        return;
    }

    QThread::msleep(diffMSecs);
    run();
}

QTime ProcessLog::totalTime()
{
    int totalMSecs = _log.constLast().time.msecsSinceStartOfDay() - _log.constFirst().time.msecsSinceStartOfDay();
    return QTime::fromMSecsSinceStartOfDay(totalMSecs);
}

QTime ProcessLog::elapsedTime()
{
    if(_logIndex < 0) {
        return QTime::fromMSecsSinceStartOfDay(0);
    } else if(_logIndex >= _log.size()) {
        return totalTime();
    }

    int elapsedMSecs = _log[_logIndex].time.msecsSinceStartOfDay() - _log.constFirst().time.msecsSinceStartOfDay();
    return QTime::fromMSecsSinceStartOfDay(elapsedMSecs);
}

ProcessLog::~ProcessLog() = default;
