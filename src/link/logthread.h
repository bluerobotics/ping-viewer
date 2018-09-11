#pragma once

#include <QByteArray>
#include <QTime>
#include <QTimer>
#include <QVector>

class LogThread : public QTimer
{
    Q_OBJECT
public:
    LogThread(QObject *parent = nullptr);
    ~LogThread();

    void startJob() { _playLog = true; start();};
    void pauseJob() { _playLog = false; };
    int packageSize() { return _log.size() - 1; };
    int packageIndex() { return _logIndex; }
    void setPackageIndex(int index) { if(_logIndex < _log.size()) _logIndex = index; }
    QTime totalTime();
    QTime elapsedTime();

    void append(QTime time, QByteArray data) { _log.append(Pack{time, data}); };

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