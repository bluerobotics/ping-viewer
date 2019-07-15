#include <QDebug>

#include "logger.h"
#include "logthread.h"

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
        start(diffMSecs);
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