#pragma once

#include <QLoggingCategory>
#include <QObject>
#include <QSettings>
#include <QTextEdit>

enum QtMsgType;
class QMessageLogContext;
class QString;

Q_DECLARE_LOGGING_CATEGORY(logger)

#define PING_LOGGING_CATEGORY(name, ...) \
    static PingLoggingCategory pingCategory ## name (__VA_ARGS__); \
    Q_LOGGING_CATEGORY(name, __VA_ARGS__)

class Logger : public QObject
{
    Q_OBJECT
public:
    void writeMessage(const QString& msg);

    static void messageHandle(QtMsgType type, const QMessageLogContext& context, const QString &msg);
    Q_PROPERTY(QString logText READ logText WRITE setLogText NOTIFY logTextChanged)
    QString logText() { return _logText; };
    void setLogText(QString text) { _logText = text;};
    Q_INVOKABLE QString consumeLogText() {QString tmp = _logText; _logText.clear(); return tmp;};

    void registerCategory(const char* category);
    Q_PROPERTY(QStringList registeredCategory READ registeredCategory NOTIFY registeredCategoryChanged)
    QStringList registeredCategory() { return _registeredCategories; };
    Q_INVOKABLE void setCategory(QString category, bool enable);
    Q_INVOKABLE bool getCategory(QString category);

    ~Logger();
    static Logger* self();

    signals:
        void logTextChanged();
        void registeredCategoryChanged();

private:
    Logger* operator = (Logger& other) = delete;
    Logger(const Logger& other) = delete;
    Logger();

    QString _logText;
    QStringList _registeredCategories;
    QSettings _settings;
};

class PingLoggingCategory
{
public:
    PingLoggingCategory(const char* category) { Logger::self()->registerCategory(category); }
};