#include "logger.h"

#include <iostream>
#include <QDebug>
#include <QString>
#include <QTime>
#include <QtConcurrent>

PING_LOGGING_CATEGORY(logger, "ping.logger")

Logger::Logger():
    _settings("Blue Robotics Inc.", "Ping Viewer")
{
    if(_settings.contains("filter")) {
        QString filter = _settings.value("filter").toString();
        QLoggingCategory::setFilterRules(filter);
    }

    /*
        Logger is a singleton, to Install the message handler
        the Logger construct need to finish,
        and that's why QtConcurrent is necessary
    */
    QtConcurrent::run([=](){qInstallMessageHandler(messageHandle);});

    registerCategory("qml");
}

void Logger::writeMessage(const QString& msg)
{
    QString debugMsg = msg;
    qDebug().noquote() << debugMsg.remove(QRegExp("<[^>]*>"));
    QString text = msg;
    _logText = _logText + msg + "<br/>";

    emit(Logger::self()->logTextChanged());
}

void Logger::messageHandle(QtMsgType type, const QMessageLogContext& context, const QString &msg)
{
    Q_UNUSED(context)

    const QString file = QString(context.file).split('/').last();
    const QString info = QString("%1 at %2(%3)").arg(context.category).arg(file).arg(context.line);

    QString txt = QTime::currentTime().toString(QStringLiteral("[hh:mm:ss:zzz] "));
    switch (type) {
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

    Logger::self()->writeMessage(txt);
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
