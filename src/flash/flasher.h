#pragma once

#include "linkconfiguration.h"

#include <QLoggingCategory>
#include <QProcess>


Q_DECLARE_LOGGING_CATEGORY(FLASH)

/**
 * @brief Manage the project Flasher
 *
 */
class Flasher : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new Flasher object
     *
     * @param parent
     */
    Flasher(QObject* parent = nullptr);

    /**
     * @brief Destroy the Flasher object
     *
     */
    ~Flasher() = default;

    /**
     * @brief Set the error type and message
     *  this function will emit an error signal and update the error property
     *
     * @param errorMessage
     */
    void setError(const QString& errorMessage);

    /**
     * @brief Return error string
     *
     * @return QString
     */
    QString error() const { return _error; };
    // Error will not be updated by the QML part
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)

    /**
     * @brief Start the flash procedure
     *
     */
    void flash();

    /**
     * @brief Set the flash baud rate
     *
     * @param baudRate
     */
    bool setBaudRate(int baudRate);

    /**
     * @brief Set the binary path
     *
     * @param firmwareFilePath
     */
    bool setFirmwarePath(const QString& firmwareFilePath);

    /**
     * @brief Set the flash link
     *
     * @param link
     */
    bool setLink(const LinkConfiguration& link);

    /**
     * @brief Enable/disable verify
     *
     * @param verify
     */
    void setVerify(bool verify);

    /**
     * @brief Defines flash state
     *
     */
    enum States {
        Error = -1,
        StartingFlash,
        Flashing,
        FlashFinished,
    } states;
    Q_ENUM(States)

signals:
    void errorChanged();
    void flashProgress(float progress);
    void flashStateChanged(Flasher::States state);

private:
    void firmwareUpdatePercentage();
    static QString stm32flashPath();

    int _baudRate = 57600;
    QString _binRelativePath;
    QString _error;
    QString _firmwareFilePath;
    QSharedPointer<QProcess> _firmwareProcess;
    LinkConfiguration _link;
    const QList<int> _validBaudRates = {57600, 115200, 230400};
    bool _verify = true;
};
