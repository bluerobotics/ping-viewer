#pragma once

#include <QDataStream>
#include <QElapsedTimer>
#include <QFile>
#include <QTime>

#include <memory>

#include "abstractlink.h"
#include "logsensorstruct.h"
#include "logthread.h"

/**
 * @brief File connection class
 *
 */
class FileLink : public AbstractLink
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new File Link object
     *
     * @param parent
     */
    FileLink(QObject* parent = nullptr);
    ~FileLink();

    /**
     * @brief Return size of file in bytes
     *
     * @return qint64
     */
    qint64 byteSize() final { return _file.bytesAvailable(); };

    /**
     * @brief Return elapsed time of log
     *
     * @return QTime
     */
    QTime elapsedTime() final { return _logThread ? _logThread->elapsedTime() : QTime(); };

    /**
     * @brief Return a human friendly error message
     *
     * @return QString
     */
    QString errorString() final { return _file.errorString(); };

    /**
     * @brief Closes connection
     *
     * @return true
     * @return false
     */
    bool finishConnection() final;

    /**
     * @brief Check if connection is open
     *
     * @return true
     * @return false
     */
    bool isOpen() final;

    /**
     * @brief Check if connection is writable
     *
     * @return true
     * @return false
     */
    bool isWritable() final { return false; };

    /**
     * @brief Return package index
     *
     * @return int
     */
    int packageIndex() final { return _logThread ? _logThread->packageIndex() : 0; };

    /**
     * @brief Return number of packages
     *
     * @return int
     */
    int packageSize() final { return _logThread ? _logThread->packageSize() : 0; };

    /**
     * @brief Pause log
     *
     */
    void pause() final { if(_logThread) _logThread->pauseJob(); };

    /**
     * @brief Set the configuration object
     *
     * @param linkConfiguration
     * @return true
     * @return false
     */
    bool setConfiguration(const LinkConfiguration& linkConfiguration) final;

    /**
     * @brief Set the package index
     *
     * @param index
     */
    void setPackageIndex(int index) { if(_logThread) _logThread->setPackageIndex(index); }

    /**
     * @brief Start log
     *
     */
    void start() final { if(_logThread) _logThread->startJob(); };

    /**
     * @brief Start connection
     *
     * @return true
     * @return false
     */
    bool startConnection() final;

    /**
     * @brief Return log total time
     *
     * @return QTime
     */
    QTime totalTime() final { return _logThread ? _logThread->totalTime() : QTime(); };

    /**
     * @brief Returns a pointer of LogSensorStruct
     *  TODO: This function and everything related to the log file handle should be done by a new class
     *
     * @return LogSensorStruct*
     */
    LogSensorStruct* logSensorStruct() { return &_logSensorStruct; };

    /**
     * @brief Return a LogSensorStruct from LinkConfiguration
     *  TODO: This function and everything related to the log file handle should be done by a new class
     *
     * @param linkConfiguration
     * @return LogSensorStruct
     */
    static LogSensorStruct staticLogSensorStruct(const LinkConfiguration& linkConfiguration);

private:
    struct Pack {
        QString time;
        QByteArray data;
    };

    QIODevice::OpenModeFlag _openModeFlag;
    QElapsedTimer _timer;

    QFile _file;
    QDataStream _inout;

    LogSensorStruct _logSensorStruct;

    std::unique_ptr<LogThread> _logThread;

    void writeData(const QByteArray& data);
    void processFile();
};
