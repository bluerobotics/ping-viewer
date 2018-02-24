#include "logger.h"

#include <iostream>
#include <QDebug>
#include <QString>
#include <QTime>
#include <QtConcurrent>

PING_LOGGING_CATEGORY(logger, "ping.logger")

static QtMessageHandler originalHandler = nullptr;

Logger::Logger():
    _settings("Blue Robotics Inc.", "Ping Viewer")
{
    if(_settings.contains("filter")) {
        QString filter = _settings.value("filter").toString();
        QLoggingCategory::setFilterRules(filter);
    }

    registerCategory("qml");
}

void Logger::installHandler()
{
    originalHandler = qInstallMessageHandler(handleMessage); // This function returns the previous message handler
}

void Logger::logMessage(const QString msg)
{
    const int line = _logModel.rowCount();
    _logModel.insertRows(line, 1);
    _logModel.setData(_logModel.index(line), msg, Qt::DisplayRole);
}

void Logger::handleMessage(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    if (originalHandler) {
        originalHandler(type, context, msg);
    }

    const QString file = QString(context.file).split('/').last();
    const QString info = QString("%1 at %2(%3)").arg(context.category).arg(file).arg(context.line);

    QString txt = QTime::currentTime().toString(QStringLiteral("[hh:mm:ss:zzz] "));
    switch (type) {
        case QtInfoMsg:
            txt.append(QStringLiteral("<font color=\"%1\">Info %2: %3</font>").arg("blue", info, msg));
            break;

        case QtDebugMsg:
            txt.append(QStringLiteral("<font color=\"%1\">Debug %2: %3</font>").arg("gray", info, msg));
            break;

        case QtWarningMsg:
            txt.append(QStringLiteral("<font color=\"%1\">Warning %2: %3</font>").arg("yellow", info, msg));
            break;

        case QtCriticalMsg:
            txt.append(QStringLiteral("<font color=\"%1\">Critical %2: %3</font>").arg("red", info, msg));
            break;

        case QtFatalMsg:
            txt.append(QStringLiteral("<font color=\"%1\">Fatal %2: %3</font>").arg("red", info, msg));
            std::cout << txt.toStdString() << std::endl;
            abort();
            break;

        default:
            return;
    }

    Logger::self()->logMessage(txt);
}

void Logger::registerCategory(const char* category)
{
    _registeredCategories << category;
    qCDebug(logger) << category << _settings.value(category).toBool();
    emit registeredCategoryChanged();
}

void Logger::setCategory(QString category, bool enable)
{
    qCDebug(logger) << category << enable;
    _settings.setValue(category, enable);

    QString filter;
    for(const auto ourCategory : _registeredCategories) {
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

Logger::~Logger()
{
    QString filter;
    for(const auto ourCategory : _registeredCategories) {
        filter += QStringLiteral("%1.debug=%2\n").arg(ourCategory, _settings.value(ourCategory).toBool() ? "true" : "false");
    }
    _settings.setValue("filter", filter);
}
