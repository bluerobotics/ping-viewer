#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QDebug>

#include "abstractlink.h"
#include "waterfall.h"
#include "logger.h"
#include "ping.h"

Q_DECLARE_LOGGING_CATEGORY(mainCategory)

PING_LOGGING_CATEGORY(mainCategory, "ping.main")

QObject *loggerRegister(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return Logger::self();
}

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Blue Robotics Inc.");
    QCoreApplication::setOrganizationDomain("bluerobotics.com");
    QCoreApplication::setApplicationName("Ping Viewer");

    QQuickStyle::setStyle("Material");

    qmlRegisterSingletonType<Logger>("Logger", 1, 0, "Logger", &loggerRegister);
    qmlRegisterType<Waterfall>("Waterfall", 1, 0, "Waterfall");
    qmlRegisterType<Ping>("Ping", 1, 0, "Ping");
    qmlRegisterType<Protocol>("Protocol", 1, 0, "Protocol");
    qmlRegisterType<AbstractLink>("AbstractLink", 1, 0, "AbstractLink");

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // Load the QML and set the Context
    // Logo
    engine.load(QUrl(QStringLiteral("qrc:/Logo.qml")));
    app.exec();
    // Main app
    qCDebug(mainCategory) << GIT_VERSION;
    qCDebug(mainCategory) << GIT_VERSION_DATE;
    qCDebug(mainCategory) << GIT_TAG;
    qCDebug(mainCategory) << GIT_URL;
    engine.rootContext()->setContextProperty("GitVersion", QStringLiteral(GIT_VERSION));
    engine.rootContext()->setContextProperty("GitVersionDate", QStringLiteral(GIT_VERSION_DATE));
    engine.rootContext()->setContextProperty("GitTag", QStringLiteral(GIT_TAG));
    engine.rootContext()->setContextProperty("GitUrl", QStringLiteral(GIT_URL));
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    return app.exec();
}
