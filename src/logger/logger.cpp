#include "logger.h"
#include "filemanager.h"

#include <fmt/color.h>
#include <fmt/core.h>

#include <QColor>
#include <QDebug>
#include <QQmlEngine>
#include <QString>
#include <QTime>
#include <QtConcurrent>
#include <iostream>

PING_LOGGING_CATEGORY(logger, "ping.logger")

Logger::Logger()
    : _file(FileManager::self()->createFileName(FileManager::Folder::GuiLogs))
    , _fileStream(&_file)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (!_file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qCWarning(logger) << "A file with the gui log will not be available !";
    }

    registerCategory("default");
    registerCategory("qml");
}

void Logger::installHandler()
{
    self()->logModel()->start();
    qInstallMessageHandler(handleMessage);

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

void Logger::logMessage(const QString& msg, const QtMsgType& type, const QMessageLogContext& context)
{
    const QString time = QTime::currentTime().toString(QStringLiteral("[hh:mm:ss:zzz]"));

    // Save the message into the file
    _fileStream << QString("%1 %2\n").arg(time, msg);

    _logModel.append(time, msg, _colors[type], _categoryIndexer[context.category]);
}

void Logger::handleMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    // Check if category is not registered and add it in Logger
    Logger::self()->registerCategory(context.category);

    static const QString msgTypes[] = {"Debug", "Warning", "Critical", "Fatal", "Info"};
    const QString file = QString(context.file).split('/').last();
    QString fileInfo;
    if (!file.isEmpty()) {
        fileInfo = QString("%1(%2) ").arg(file).arg(context.line);
    }

    const QString logMsg = QString("%1[%2]: %3%4").arg(context.category, msgTypes[type], fileInfo, msg);

    Logger::self()->logMessage(logMsg, type, context);

    fmt::text_style style;
    switch (type) {
    case QtDebugMsg:
        style = fg(fmt::terminal_color::bright_green);
        break;
    case QtInfoMsg:
        style = fmt::emphasis::bold | fg(fmt::color::white);
        break;
    case QtWarningMsg:
        style = fmt::emphasis::bold | fg(fmt::color::yellow);
        break;
    case QtCriticalMsg:
        style = fmt::emphasis::bold | fg(fmt::color::red);
        break;
    case QtFatalMsg:
        style = fmt::emphasis::bold | fg(fmt::color::yellow) | bg(fmt::color::red);
        break;
    }
    fmt::print(style, "{}\n", qFormatLogMessage(type, context, logMsg).toStdString());
}

void Logger::registerCategory(const char* category)
{
    if (_registeredCategories.contains(category)) {
        return;
    }
    // Register each category in a bit, this will help the qml element to be faster when searching between categories
    _categoryIndexer[category] = 1 << _categoryIndexer.size();

    qCDebug(logger) << "New category registered: " << category;
    _registeredCategories << category;
    emit registeredCategoryChanged();
}

uint Logger::getCategoryIndex(const QString& category) { return _categoryIndexer[category]; }

Logger* Logger::self()
{
    static Logger self;
    return &self;
}

void Logger::test()
{
    qInfo() << "This is an info message";
    qDebug() << "This is a debug message";
    qWarning() << "This is a warning message";
    qCritical() << "This is a critical message";
}

Logger::~Logger() = default;
