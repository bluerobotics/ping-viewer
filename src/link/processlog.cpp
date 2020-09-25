#include <QDebug>

#include "logger.h"
#include "processlog.h"

PING_LOGGING_CATEGORY(PING_PROCESSLOG, "ping.ProcessLog");

ProcessLog::ProcessLog(QObject* parent)
    : QObject(parent)
    , _logIndex(0)
    , _play(true)
    , _replayTimeMs(0)
    , _sleepTime(0)
    , _stop(false)
{
}

void ProcessLog::run()
{
    int diffMSecs = 0;
    int lastMSecs = 0;

    while (!_stop) {
        // Check for pause condition and valid log index
        if (!_play) {
            QThread::msleep(200);
            continue;
        }

        const QByteArray& data = _log[_logIndex].data;
        lastMSecs = _log[_logIndex].time.msecsSinceStartOfDay();
        emit packageIndexChanged(_logIndex);
        emit newPackage(data);

        _logIndex++;
        // Check if we have data before sending
        if (_logIndex >= _log.size()) {
            qCDebug(PING_PROCESSLOG) << "End of the log.";

            // Restart thread and wait for user interaction
            _logIndex = 0;
            _play = false;
            continue;
        }

        diffMSecs = _log[_logIndex].time.msecsSinceStartOfDay() - lastMSecs;

        // Something is wrong, we need to go 'back to the future'
        if (diffMSecs < 0) {
            qCWarning(PING_PROCESSLOG) << "Sample time is negative from previous sample! Trying to recover..";
            qCDebug(PING_PROCESSLOG) << "First time:" << _log.constFirst().time;
            qCDebug(PING_PROCESSLOG) << "Last time:" << _log.constLast().time;
            qCDebug(PING_PROCESSLOG) << "Actual index:" << _logIndex << "Time[n-1, n]:" << _log[_logIndex - 1].time
                                     << _log[_logIndex].time;
            continue;
        }


        _sleepTime = _replayTimeMs ? _replayTimeMs.load() : diffMSecs;
        while(_sleepTime > 0) {
            QThread::msleep(10);
            _sleepTime -= 10;
        }
    }
}

QTime ProcessLog::totalTime()
{
    int totalMSecs = _log.constLast().time.msecsSinceStartOfDay() - _log.constFirst().time.msecsSinceStartOfDay();
    return QTime::fromMSecsSinceStartOfDay(totalMSecs);
}

QTime ProcessLog::elapsedTime()
{
    if (_logIndex < 0) {
        return QTime::fromMSecsSinceStartOfDay(0);
    } else if (_logIndex >= _log.size()) {
        return totalTime();
    }

    int elapsedMSecs = _log[_logIndex].time.msecsSinceStartOfDay() - _log.constFirst().time.msecsSinceStartOfDay();
    return QTime::fromMSecsSinceStartOfDay(elapsedMSecs);
}

ProcessLog::~ProcessLog() = default;
