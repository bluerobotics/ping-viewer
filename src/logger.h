#pragma once

#include <QLoggingCategory>
#include <QSettings>
#include <QStringListModel>

#include "loglistmodel.h"

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
    static void handleMessage(QtMsgType type, const QMessageLogContext& context, const QString &msg);
    Q_PROPERTY(QStringListModel* logModel READ logModel CONSTANT)
    QStringListModel* logModel() { return &_logModel; };

    void registerCategory(const char* category);
    Q_PROPERTY(QStringList registeredCategory READ registeredCategory NOTIFY registeredCategoryChanged)
    QStringList registeredCategory() { return _registeredCategories; };
    Q_INVOKABLE void setCategory(QString category, bool enable);
    Q_INVOKABLE bool getCategory(QString category);

    ~Logger();
    static Logger* self();
    static void installHandler();
    static void test();

signals:
    void registeredCategoryChanged();

private:
    Logger* operator = (Logger& other) = delete;
    Logger(const Logger& other) = delete;
    Logger();

    void logMessage(const QString& msg, QtMsgType type);

    QStringList _registeredCategories;
    QSettings _settings;
    LogListModel _logModel;
};

class PingLoggingCategory
{
public:
    PingLoggingCategory(const char* category) { Logger::self()->registerCategory(category); }
};
