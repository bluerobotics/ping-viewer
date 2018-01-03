#pragma once

#include <QObject>
#include <QTextEdit>

enum QtMsgType;
class QMessageLogContext;
class QString;

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

    ~Logger();
    static Logger* self();

    signals:
        void logTextChanged();

private:
    Logger* operator = (Logger& other) = delete;
    Logger(const Logger& other) = delete;
    Logger();

    QString _logText;
};
