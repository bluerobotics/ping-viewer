#include "filemanager.h"
#include "logger.h"
#include "settingsmanager.h"

#include <iostream>
#include <QDebug>
#include <QString>
#include <QTime>
#include <QtConcurrent>
#include <QColor>

PING_LOGGING_CATEGORY(logger, "ping.logger")

static QtMessageHandler originalHandler = nullptr;

Logger::Logger()
    : _file(FileManager::self()->createFileName(FileManager::Folder::GuiLogs))
    , _fileStream(&_file)
    , _settings(SettingsManager::self()->settings())
{
    if(!_file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qCWarning(logger) << "A file with the gui log will not be available !";
    }

    if(_settings.contains("filter")) {
        QString filter = _settings.value("filter").toString();
        QLoggingCategory::setFilterRules(filter);
    }

    registerCategory("qml");
}

void Logger::installHandler()
{
    originalHandler = qInstallMessageHandler(handleMessage); // This function returns the previous message handler

    if (qEnvironmentVariableIsEmpty("QT_MESSAGE_PATTERN")) {
        qSetMessagePattern(QStringLiteral("%{time [hh:mm:ss.zzz]} %{message}"));
    }
}

QObject* Logger::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}

void Logger::logMessage(const QString& msg, QtMsgType type)
{
    const QString time = QTime::currentTime().toString(QStringLiteral("[hh:mm:ss:zzz]"));

    // Save the message into the file
    _fileStream << QString("%1 %2\n").arg(time, msg);

    const int line = _logModel.rowCount();
    // Debug, Warning, Critical, Fatal, Info
    static const QColor colors[] = { QColor("DarkGray"), QColor("orange"), QColor("red"), QColor("red"), QColor("LimeGreen") };

    _logModel.insertRows(line, 1);
    _logModel.setData(_logModel.index(line), time, LogListModel::TimeRole);
    _logModel.setData(_logModel.index(line), msg, Qt::DisplayRole);
    _logModel.setData(_logModel.index(line), colors[type], Qt::ForegroundRole);
}

void Logger::handleMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    // Check if category is not registered and add it in Logger
    Logger::self()->registerCategory(context.category);

    static const QString msgTypes[] = { "Debug", "Warning", "Critical", "Fatal", "Info" };
    const QString file = QString(context.file).split('/').last();
    QString fileInfo;
    if (!file.isEmpty()) {
        fileInfo = QString("%1(%2) ").arg(file).arg(context.line);
    }

    const QString logMsg = QString("%1[%2]: %3%4").arg(context.category, msgTypes[type], fileInfo, msg);

    Logger::self()->logMessage(logMsg, type);
    if (originalHandler) {
        originalHandler(type, context, logMsg);
    }
}

void Logger::registerCategory(const char* category)
{
    if (_registeredCategories.contains(category)) {
        return;
    }
    qCDebug(logger) << "New category registered: " << category;
    _registeredCategories << category;
    qCDebug(logger) << category << _settings.value(category).toBool();
    emit registeredCategoryChanged();
}

void Logger::setCategory(QString category, bool enable)
{
    qCDebug(logger) << category << enable;
    _settings.setValue(category, enable);

    QString filter;
    for(const auto& ourCategory : qAsConst(_registeredCategories)) {
        filter += QStringLiteral("%1.debug=%2\n").arg(ourCategory, _settings.value(ourCategory).toBool() ? "true" : "false");
    }

    qCDebug(logger) << "filter" << filter;
    QLoggingCategory::setFilterRules(filter);
    _settings.setValue("filter", filter);
}

bool Logger::getCategory(QString category)
{
    qCDebug(logger) << category << _settings.value(category).toBool();
    return _settings.value(category).toBool();
};

Logger* Logger::self()
{
    static Logger* self = new Logger();
    return self;
}

void Logger::test()
{
    qInfo() << "This is an info message";
    qDebug() << "This is a debug message";
    qWarning() << "This is a warning message";
    qCritical() << "This is a critical message";
}

Logger::~Logger() = default;
