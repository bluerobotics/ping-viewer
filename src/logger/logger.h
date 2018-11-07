#pragma once

#include <QFile>
#include <QLoggingCategory>
#include <QSettings>
#include <QStringListModel>

#include "loglistmodel.h"

enum QtMsgType;
class QJSEngine;
class QMessageLogContext;
class QQmlEngine;
class QString;

Q_DECLARE_LOGGING_CATEGORY(logger)

/**
 * @brief This is a macro to register our categories
 *
 */
#define PING_LOGGING_CATEGORY(name, ...) \
    static PingLoggingCategory pingCategory ## name (__VA_ARGS__); \
    Q_LOGGING_CATEGORY(name, __VA_ARGS__)

/**
 * @brief Manage the project logger
 *
 */
class Logger : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Handle new messages
     *
     * @param type
     * @param context
     * @param msg
     */
    static void handleMessage(QtMsgType type, const QMessageLogContext& context, const QString &msg);

    /**
     * @brief Get the log model
     *
     * @return QStringListModel*
     */
    QStringListModel* logModel() { return &_logModel; };
    Q_PROPERTY(QStringListModel* logModel READ logModel CONSTANT)

    /**
     * @brief Register a new category to be tracked
     *
     * @param category
     */
    void registerCategory(const char* category);

    /**
     * @brief Get a list of already registered categories
     *
     * @return QStringList
     */
    QStringList registeredCategory() { return _registeredCategories; };
    Q_PROPERTY(QStringList registeredCategory READ registeredCategory NOTIFY registeredCategoryChanged)

    /**
     * @brief Return the index of the category
     *
     * @param category
     * @return uint
     */
    Q_INVOKABLE uint getCategoryIndex(QString category);

    /**
     * @brief Register this class to handle the messages
     *
     */
    static void installHandler();

    /**
     * @brief Test messages types
     *
     */
    static void test();

    /**
     * @brief Return Logger pointer
     *
     * @return Logger*
     */
    static Logger* self();
    ~Logger();

    /**
     * @brief Return a pointer of this singleton to the qml register function
     *
     * @param engine
     * @param scriptEngine
     * @return QObject*
     */
    static QObject* qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine);

signals:
    void registeredCategoryChanged();

private:
    Q_DISABLE_COPY(Logger)
    /**
     * @brief Construct a new Logger object
     *
     */
    Logger();

    /**
     * @brief Log message in the model
     *
     * @param msg
     * @param type
     */
    void logMessage(const QString& msg, const QtMsgType& type, const QMessageLogContext& context);

    QMap<QString, uint> _categoryIndexer;
    QFile _file;
    QTextStream _fileStream;
    QStringList _registeredCategories;
    LogListModel _logModel;
};

/**
 * @brief Register categories
 *
 */
class PingLoggingCategory
{
public:
    /**
     * @brief Register a new category in Logger
     *
     * @param category
     */
    PingLoggingCategory(const char* category) { Logger::self()->registerCategory(category); }
};
