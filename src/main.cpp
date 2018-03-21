#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QDebug>

#include "abstractlink.h"
#include "logger.h"
#include "ping.h"
#include "util.h"
#include "waterfall.h"

Q_DECLARE_LOGGING_CATEGORY(mainCategory)

PING_LOGGING_CATEGORY(mainCategory, "ping.main")

QObject *loggerRegister(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return Logger::self();
}

QObject *utilRegister(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return Util::self();
}

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Blue Robotics Inc.");
    QCoreApplication::setOrganizationDomain("bluerobotics.com");
    QCoreApplication::setApplicationName("Ping Viewer");

    QQuickStyle::setStyle("Material");

    qmlRegisterSingletonType<Logger>("Logger", 1, 0, "Logger", &loggerRegister);
    qmlRegisterSingletonType<Util>("Util", 1, 0, "Util", &utilRegister);
    qmlRegisterType<Waterfall>("Waterfall", 1, 0, "Waterfall");
    qmlRegisterType<Ping>("Ping", 1, 0, "Ping");
    qmlRegisterType<Protocol>("Protocol", 1, 0, "Protocol");
    qmlRegisterType<AbstractLink>("AbstractLink", 1, 0, "AbstractLink");

    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // Load the QML and set the Context
    // Logo
#ifdef QT_NO_DEBUG
    engine.load(QUrl(QStringLiteral("qrc:/Logo.qml")));
    app.exec();
#endif
    // Main app
    engine.rootContext()->setContextProperty("GitVersion", QStringLiteral(GIT_VERSION));
    engine.rootContext()->setContextProperty("GitVersionDate", QStringLiteral(GIT_VERSION_DATE));
    engine.rootContext()->setContextProperty("GitTag", QStringLiteral(GIT_TAG));
    engine.rootContext()->setContextProperty("GitUrl", QStringLiteral(GIT_URL));
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    Logger::installHandler();
    qCInfo(mainCategory) << GIT_VERSION;
    qCInfo(mainCategory) << GIT_VERSION_DATE;
    qCInfo(mainCategory) << GIT_TAG;
    qCInfo(mainCategory) << GIT_URL;
    return app.exec();
}
