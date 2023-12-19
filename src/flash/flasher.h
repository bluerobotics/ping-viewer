#pragma once

#include "linkconfiguration.h"

#include <QLoggingCategory>
#include <QProcess>

Q_DECLARE_LOGGING_CATEGORY(FLASH)

/**
 * @brief Manage the project Flasher
 *
 */
class Flasher : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Construct a new Flasher object
     *
     * @param parent
     * @param validBaudRates
     */
    Flasher(QObject* parent = nullptr);

    /**
     * @brief Destroy the Flasher object
     *
     */
    ~Flasher() = default;

    /**
     * @brief Defines flash state
     *
     */
    enum States {
        Error = -1,
        Idle,
        StartingFlash,
        Flashing,
        FlashFinished,
    };
    Q_ENUM(States)

    /**
     * @brief Set state message
     *  this function will emit a message signal and update the message property
     *
     * @param message
     */
    void setMessage(const QString& message);

    /**
     * @brief Return message string
     *
     * @return QString
     */
    QString message() const { return _message; };
    Q_PROPERTY(QString message READ message NOTIFY messageChanged)

    /**
     * @brief Set flasher state and optionally message
     *
     * @param state
     * @param message
     */
    void setState(Flasher::States state, QString message = QString());

    /**
     * @brief Return flasher state
     *
     * @return Flasher::States
     */
    Flasher::States state() const { return _state; };
    Q_PROPERTY(Flasher::States state READ state NOTIFY stateChanged)

    /**
     * @brief Return the valid baud rates for device communication
     */
    const QVariantList& validBaudRates() const { return _validBaudRates; };
    Q_PROPERTY(QVariant validBaudRates READ validBaudRates CONSTANT);

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

signals:
    void messageChanged();
    void flashProgress(float progress);
    void stateChanged(Flasher::States state);

private:
    void firmwareUpdatePercentage(const QString& output);
    static QString stm32flashPath();

    int _baudRate = 57600;
    QString _binRelativePath;
    QString _firmwareFilePath;
    QSharedPointer<QProcess> _firmwareProcess;
    LinkConfiguration _link;
    QString _message;
    States _state = Idle;
    const QList<QVariant> _validBaudRates = {57600, 115200, 230400};
    bool _verify = true;
};
