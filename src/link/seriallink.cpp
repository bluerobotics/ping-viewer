#include <QDebug>
#include <QLoggingCategory>
#include <QSerialPortInfo>
#include <QThread>
#include <QTimer>

#include "logger.h"
#include "seriallink.h"

#ifdef Q_OS_LINUX
#include <linux/serial.h>
#include <sys/ioctl.h>
#include <termios.h>
#endif
#ifdef Q_OS_WIN
#include <QSettings>
#include <Windows.h>
#endif

PING_LOGGING_CATEGORY(PING_PROTOCOL_SERIALLINK, "ping.protocol.seriallink")

SerialLink::SerialLink(QObject* parent)
    : AbstractLink("SerialLink", parent)
{
    setType(LinkType::Serial);

    connect(
        &_port, &QIODevice::readyRead, this, [this]() { emit newData(_port.readAll()); }, Qt::DirectConnection);

    connect(
        this, &AbstractLink::sendData, this,
        [this](const QByteArray& data) {
            _port.write(data);
            _port.flush();
        },
        Qt::DirectConnection);

    connect(&_port, &QSerialPort::errorOccurred, this, [this](QSerialPort::SerialPortError error) {
        switch (error) {
        case QSerialPort::NoError:
            break;
        default:
            qCWarning(PING_PROTOCOL_SERIALLINK) << "Error is critical ! Port need to be closed.";
            qCWarning(PING_PROTOCOL_SERIALLINK) << "Error:" << error;
            finishConnection();
            break;
        }
    });
}

bool SerialLink::setConfiguration(const LinkConfiguration& linkConfiguration)
{
    _linkConfiguration = linkConfiguration;
    qCDebug(PING_PROTOCOL_SERIALLINK) << linkConfiguration;
    if (!linkConfiguration.isValid()) {
        qCDebug(PING_PROTOCOL_SERIALLINK) << LinkConfiguration::errorToString(linkConfiguration.error());
        return false;
    }

    setName(linkConfiguration.name());

    _port.setPortName(linkConfiguration.args()->at(0));
    _port.setBaudRate(linkConfiguration.args()->at(1).toInt());
    emit configurationChanged();
    return true;
}

bool SerialLink::startConnection()
{
    // Check if port was already open
    if (isOpen()) {
        qCDebug(PING_PROTOCOL_SERIALLINK) << "Serial port will be restarted.";
        finishConnection();
    }

    if (!_port.open(QIODevice::ReadWrite)) {
        qCWarning(PING_PROTOCOL_SERIALLINK) << "Fail to open serial port:" << _port.error();
        return false;
    }

    forceSensorAutomaticBaudRateDetection();

    return true;
}

bool SerialLink::finishConnection()
{
    if (_port.isOpen()) {
        _port.close();
        qCDebug(PING_PROTOCOL_SERIALLINK) << "Port closed.";
    }
    return true;
}

QStringList SerialLink::listAvailableConnections()
{
    static QStringList list;
    auto oldList = list;
    list.clear();
    auto ports = QSerialPortInfo::availablePorts();
    for (const auto& port : ports) {
        list.append(port.portName());
    }
    if (oldList != list) {
        emit availableConnectionsChanged();
    }
    return list;
}

void SerialLink::setBaudRate(int baudRate)
{
    _port.close();
    _port.setBaudRate(baudRate);
    startConnection();
    setLowLatency();

    QStringList args = _linkConfiguration.argsAsConst();
    args[1] = QString::number(baudRate);
    _linkConfiguration.setArgs(args);
    forceSensorAutomaticBaudRateDetection();
}

#ifdef Q_OS_MACOS
bool SerialLink::setLowLatency()
{
    qCWarning(PING_PROTOCOL_SERIALLINK) << "No support for low latency mode in macOS.";
    // no light at the end of the tunnel for macOS
    return false;
}
#endif

#ifdef Q_OS_LINUX
bool SerialLink::setLowLatency()
{
    auto handle = _port.handle();

    if (!handle) {
        qCWarning(PING_PROTOCOL_SERIALLINK) << "Failed to get serial handle from OS.";
        return false;
    }

    // Our first attempt is with termios2
    struct termios2 {
        tcflag_t c_iflag; /* input mode flags */
        tcflag_t c_oflag; /* output mode flags */
        tcflag_t c_cflag; /* control mode flags */
        tcflag_t c_lflag; /* local mode flags */
        cc_t c_line; /* line discipline */
        cc_t c_cc[19]; /* control characters */
        speed_t c_ispeed; /* input speed */
        speed_t c_ospeed; /* output speed */
    } tio2;
    if (::ioctl(handle, TCGETS2, &tio2) != -1) {
        // If it's already in low latency, no further configuration is necessary
        if (!(tio2.c_cflag & ASYNC_LOW_LATENCY)) {
            tio2.c_cflag |= ASYNC_LOW_LATENCY;
            ::ioctl(handle, TCSETS2, &tio2);
        } else {
            qCDebug(PING_PROTOCOL_SERIALLINK) << "Low latency mode is already in termios2.";
        }
    } else {
        qCWarning(PING_PROTOCOL_SERIALLINK) << "Failed to get termios2 struct from system.";
    }

    // Check again same configuration with serial_struct
    serial_struct serial;
    ::memset(&serial, 0, sizeof(serial));
    if (::ioctl(handle, TIOCGSERIAL, &serial) == -1) {
        qCWarning(PING_PROTOCOL_SERIALLINK) << "Failed to get serial_struct from system.";
        return false;
    }

    // If it's already in low latency, no further configuration is necessary
    if (serial.flags & ASYNC_LOW_LATENCY) {
        qCDebug(PING_PROTOCOL_SERIALLINK) << "Low latency mode is already in serial_struct.";
        return true;
    }

    // It's not possible to check for errors since the driver may not support it
    serial.flags |= ASYNC_LOW_LATENCY;
    ::ioctl(handle, TIOCSSERIAL, &serial);

    // light at the end of the tunnel
    return true;
}
#endif

#ifdef Q_OS_WIN
// TODO: On windows the change of latency timer works in the second attempt sometimes.
// This needs further investigation and improvement.
bool SerialLink::setLowLatency()
{
    auto handle = _port.handle();

    if (!handle) {
        qCWarning(PING_PROTOCOL_SERIALLINK) << "Failed to get serial handle from OS.";
        return false;
    }

    if (handle) {
        // Configure timeout
        COMMTIMEOUTS currentCommTimeouts;
        ::ZeroMemory(&currentCommTimeouts, sizeof(currentCommTimeouts));

        if (::GetCommTimeouts(handle, &currentCommTimeouts)) {
            currentCommTimeouts.ReadIntervalTimeout = 1;
            if (!::SetCommTimeouts(handle, &currentCommTimeouts)) {
                qCWarning(PING_PROTOCOL_SERIALLINK) << "Failed to set read interval timeout.";
                qCDebug(PING_PROTOCOL_SERIALLINK) << "System error:" << ::GetLastError();
            }
        } else {
            qCWarning(PING_PROTOCOL_SERIALLINK) << "Failed to get COMMTIMEOUTS struct from system.";
            qCDebug(PING_PROTOCOL_SERIALLINK) << "System error:" << ::GetLastError();
        }
    } else {
        qCWarning(PING_PROTOCOL_SERIALLINK) << "Failed to get serial handle from OS.";
    }

    // Set low latency mode with windows register system
    // HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\FTDIBUS\VID_XXXX+PID_XXXX+ABCDEFGH\0000\Device
    // Parameters\LatencyTimer

    // Create VID/PID/serial number register string path
    QSerialPortInfo serialPortInfo(_port);
    const QString deviceSettingsIdentification = QStringLiteral("VID_%1+PID_%2+%3")
                                                     .arg(serialPortInfo.vendorIdentifier(), 4, 16, QChar('0'))
                                                     .arg(serialPortInfo.productIdentifier(), 4, 16, QChar('0'))
                                                     .arg(serialPortInfo.serialNumber());
    const QString settingsSystemKey
        = QStringLiteral(R"(HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Enum\FTDIBUS\%1\0000\Device Parameters\)")
              .arg(deviceSettingsIdentification);
    qCDebug(PING_PROTOCOL_SERIALLINK) << "Register path:" << settingsSystemKey;

    // Change register LatencyTimer value to 1ms
    QSettings settings(settingsSystemKey, QSettings::NativeFormat);

    // Variable exist, so our path is valid
    QVariant latencyTimerValue = settings.value("LatencyTimer");
    if (!latencyTimerValue.isValid()) {
        qCWarning(PING_PROTOCOL_SERIALLINK) << "Variable does not exist, register path is wrong.";
        return false;
    }

    if (latencyTimerValue.toInt() == 1) {
        qCDebug(PING_PROTOCOL_SERIALLINK) << "Low latency timer is already in register.";
        return true;
    }

    if (!settings.isWritable()) {
        qCWarning(PING_PROTOCOL_SERIALLINK) << "Register is not writable.";
        return false;
    }

    settings.setValue("LatencyTimer", 1);
    settings.sync();
    auto settingsStatus = settings.status();
    if (settingsStatus != QSettings::NoError) {
        qCWarning(PING_PROTOCOL_SERIALLINK) << "Failed to save LatencyTimer setting.";
        qCDebug(PING_PROTOCOL_SERIALLINK) << "Settings status:" << settingsStatus;
        return false;
    }

    // light at the end of the tunnel
    return true;
}
#endif

void SerialLink::forceSensorAutomaticBaudRateDetection()
{
    /** ABR fluxogram
     * 1. Use break to force a 0 logical state for an entire frame
     * 2. Send U (0b01010101) to allow an automatic baud rate detection
     * 3. Force a write condition in the serial using the `flush` command
     */
    _port.setBreakEnabled(true);
    QThread::msleep(10);
    _port.setBreakEnabled(false);
    QThread::usleep(10);
    _port.write(QByteArray("U").repeated(10));
    _port.flush();
    QThread::msleep(11);
}

SerialLink::~SerialLink() { finishConnection(); }
