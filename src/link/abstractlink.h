#pragma once

#include <QObject>
#include <QTime>
#include <QTimer>

#include "linkconfiguration.h"

/**
 * @brief The abstract connection link base class
 *  This should be used in all connection types
 *
 */
class AbstractLink : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new Abstract Link object
     *
     * @param name
     * @param parent
     */
    AbstractLink(const QString& name, QObject* parent = nullptr);

    /**
     * @brief Destroy the Abstract Link object
     *
     */
    ~AbstractLink();

    /**
     * @brief Control offline source size
     *
     * @return qint64
     */
    Q_INVOKABLE virtual qint64 byteSize() { return 0; };

    /**
     * @brief Return the link configuration pointer
     *
     * @return LinkConfiguration*
     */
    LinkConfiguration* configuration() { return &_linkConfiguration; }

    /**
     * @brief Return elapsed time of connection
     *
     * @return QTime
     */
    Q_INVOKABLE virtual QTime elapsedTime() { return QTime(); };

    /**
     * @brief Return elapsed time in string format
     *
     * @return QString
     */
    Q_INVOKABLE virtual QString elapsedTimeString() { return elapsedTime().toString(_timeFormat); };

    /**
     * @brief Return error in a human friendly message
     *
     * @return QString
     */
    virtual QString errorString() { return QString(); };

    /**
     * @brief Finish connection
     *
     * @return bool
     */
    Q_INVOKABLE virtual bool finishConnection() { return true;};

    /**
     * @brief Check if connection is auto connected
     *
     * @return true
     * @return false
     */
    bool isAutoConnect() { return _autoConnect; }

    /**
     * @brief Check if connection is open
     *
     * @return bool
     */
    Q_INVOKABLE virtual bool isOpen() { return true; };

    /**
     * @brief is writable
     *
     * @return bool
     */
    Q_INVOKABLE virtual bool isWritable() { return true; };

    /**
     * @brief Return a list of available connections
     * TODO: Move this from QStringList to LinkConfiguration
     *
     * @return QStringList
     */
    Q_INVOKABLE virtual QStringList listAvailableConnections() { return QStringList(); };

    /**
     * @brief Return the package size
     *
     * @return int
     */
    Q_INVOKABLE virtual int packageSize() { return 0; };

    /**
     * @brief Return package index
     *
     * @return int
     */
    Q_INVOKABLE virtual int packageIndex() { return 0; };

    /**
     * @brief Pause connection
     *
     */
    Q_INVOKABLE virtual void pause() {};

    /**
     * @brief Return the link name
     *
     * @return const QString
     */
    const QString name() { return _name; }

    /**
     * @brief Set the auto connection state
     *
     * @param autoc
     */
    virtual void setAutoConnect(bool autoc = true) { _autoConnect = autoc; emit autoConnectChanged(); }

    /**
     * @brief Set the configuration object
     *
     * @param linkConfiguration
     * @return true
     * @return false
     */
    virtual bool setConfiguration(const LinkConfiguration& linkConfiguration) { Q_UNUSED(linkConfiguration) return true; }

    /**
     * @brief Set the link name
     *
     * @param name
     */
    void setName(const QString& name) { _name = name; emit nameChanged(_name); };

    /**
     * @brief Set package index
     *
     * @param index
     */
    Q_INVOKABLE virtual void setPackageIndex(int index) { Q_UNUSED(index) };

    /**
     * @brief Set link type
     *
     * @param type
     */
    virtual void setType(LinkType type) { _type = type; emit linkChanged(_type); };

    /**
     * @brief Start communication
     *
     */
    Q_INVOKABLE virtual void start() {};

    /**
     * @brief Start connection
     *
     */
    Q_INVOKABLE virtual bool startConnection() { return true;};

    /**
     * @brief Return total time
     *
     * @return QTime
     */
    Q_INVOKABLE virtual QTime totalTime() { return QTime(); };

    /**
     * @brief Return total time in string
     *
     * @return QString
     */
    Q_INVOKABLE virtual QString totalTimeString() { return totalTime().toString(_timeFormat); };

    /**
     * @brief Return LinkType
     *
     * @return LinkType
     */
    LinkType type() { return _type; };

    /**
     * @brief Ask link to write the bytearray
     *
     * @param data
     */
    void write(const QByteArray& data) { emit sendData(data); };

    /**
     * @brief Ask link to write a char*
     *
     * @param data
     * @param size
     */
    void write(const char* data, int size)
    {
        if(size > 0) {
            emit sendData(QByteArray(data, size));
        };
    }

    /**
     * @brief Download speed of link in bits
     *
     * @return float
     */
    float downSpeed()
    {
        return _bitRateDownSpeed.speed;
    }

    /**
     * @brief Upload speed of link in bits
     *
     * @return float
     */
    float upSpeed()
    {
        return _bitRateUpSpeed.speed;
    }

    Q_PROPERTY(qint64 byteSize READ byteSize NOTIFY byteSizeChanged)
    Q_PROPERTY(LinkConfiguration* configuration READ configuration NOTIFY configurationChanged)
    Q_PROPERTY(QTime elapsedTime READ elapsedTime NOTIFY elapsedTimeChanged)
    Q_PROPERTY(QString elapsedTimeString READ elapsedTimeString NOTIFY elapsedTimeChanged)
    Q_PROPERTY(bool isAutoConnect READ isAutoConnect WRITE setAutoConnect NOTIFY autoConnectChanged)
    Q_PROPERTY(QStringList listAvailableConnections READ listAvailableConnections NOTIFY availableConnectionsChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(int packageIndex READ packageIndex WRITE setPackageIndex NOTIFY packageIndexChanged)
    Q_PROPERTY(int packageSize READ packageSize NOTIFY packageSizeChanged)
    Q_PROPERTY(QTime totalTime READ totalTime NOTIFY totalTimeChanged)
    Q_PROPERTY(QString totalTimeString READ totalTimeString NOTIFY totalTimeChanged)
    Q_PROPERTY(AbstractLinkNamespace::LinkType type READ type WRITE setType NOTIFY linkChanged)
    Q_PROPERTY(float upSpeed READ upSpeed NOTIFY linkChanged)
    Q_PROPERTY(float downSpeed READ downSpeed NOTIFY linkChanged)

signals:
    void availableConnectionsChanged();
    void configurationChanged();
    void nameChanged(const QString& name);
    void autoConnectChanged();
    void linkChanged(AbstractLinkNamespace::LinkType link);
    void newData(const QByteArray& data);
    void sendData(const QByteArray& data);
    void speedChanged();

    void byteSizeChanged();
    void packageSizeChanged();
    void packageIndexChanged();
    void totalTimeChanged();
    void elapsedTimeChanged();

protected:
    static const QString _timeFormat;
    LinkConfiguration _linkConfiguration;

private:
    bool _autoConnect;
    QString _name;
    QTimer _oneSecondTimer;
    LinkType _type;

    // Up and down speed logic
    struct BitRateSpeed {
        float speed;
        float numberOfBytes;

        /**
         * @brief Reset total number of bytes and set the current speed
         *
         */
        void update()
        {
            speed = numberOfBytes;
            numberOfBytes = 0;
        }
    } _bitRateUpSpeed, _bitRateDownSpeed;
};
