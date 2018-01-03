#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>

#include "logger.h"

QObject *loggerRegister(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return Logger::self();
}

int main(int argc, char *argv[])
{
    qInstallMessageHandler(Logger::self()->messageHandle);

    qmlRegisterSingletonType<Logger>("Logger", 1, 0, "Logger", &loggerRegister);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // Load the QML and set the Context
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
