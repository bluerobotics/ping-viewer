#include "logger.h"

#include <QDebug>
#include <QString>
#include <QTime>
#include <QTimer>

Logger::Logger()
{
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

    QString msgCopy = QString("%1 at %2:%3 - %4").arg(context.category).arg(context.file).arg(context.line).arg(msg);

    QString txt = QTime::currentTime().toString(QStringLiteral("[hh:mm:ss:zzz] "));
    switch (type) {
        case QtDebugMsg:
            txt.append(QStringLiteral("<font color=\"%1\">Debug: %2</font>").arg("gray", msgCopy));
            break;

        case QtWarningMsg:
            txt.append(QStringLiteral("<font color=\"%1\">Warning: %2</font>").arg("yellow", msgCopy));
            break;

        case QtCriticalMsg:
            txt.append(QStringLiteral("<font color=\"%1\">Critical: %2</font>").arg("red", msgCopy));
            break;

        case QtFatalMsg:
            txt.append(QStringLiteral("<font color=\"%1\">Fatal: %2</font>").arg("red", msgCopy));
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
