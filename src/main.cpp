#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QDebug>

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

    QQuickStyle::setStyle("Material");

    qmlRegisterSingletonType<Logger>("Logger", 1, 0, "Logger", &loggerRegister);

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // Load the QML and set the Context
    // Logo
    engine.load(QUrl(QStringLiteral("qrc:/Logo.qml")));
    app.exec();
    // Main app
    qDebug() << GIT_VERSION;
    qDebug() << GIT_VERSION_DATE;
    qDebug() << GIT_TAG;
    qDebug() << GIT_URL;
    engine.rootContext()->setContextProperty("GitVersion", QStringLiteral(GIT_VERSION));
    engine.rootContext()->setContextProperty("GitVersionDate", QStringLiteral(GIT_VERSION_DATE));
    engine.rootContext()->setContextProperty("GitTag", QStringLiteral(GIT_TAG));
    engine.rootContext()->setContextProperty("GitUrl", QStringLiteral(GIT_URL));
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
