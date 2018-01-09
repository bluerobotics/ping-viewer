#include "logger.h"

#include <QDebug>
#include <QString>
#include <QTime>
#include <QTimer>

PING_LOGGING_CATEGORY(logger, "ping.logger")

Logger::Logger()
{
    qCDebug(logger) << "Starting Logger.";
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
            abort();
            break;

        default:
            return;
    }

    Logger::self()->writeMessage(txt);
}

Logger* Logger::self()
{
    static Logger* self = new Logger();
    return self;
}

Logger::~Logger()
{
}
