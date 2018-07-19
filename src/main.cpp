#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QDebug>
#include <QRegularExpression>

#include "abstractlink.h"
#include "filemanager.h"
#include "logger.h"
#include "ping.h"
#include "settingsmanager.h"
#include "util.h"
#include "waterfall.h"

// Register message enums to qml
#include "pingmessage/pingmessage.h"

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

QObject *settingsManager(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return SettingsManager::self();
}

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Blue Robotics Inc.");
    QCoreApplication::setOrganizationDomain("bluerobotics.com");
    QCoreApplication::setApplicationName("Ping Viewer");

    QQuickStyle::setStyle("Material");

    qmlRegisterSingletonType<FileManager>("FileManager", 1, 0, "FileManager", FileManager::qmlSingletonRegister);
    qmlRegisterSingletonType<Logger>("Logger", 1, 0, "Logger", &loggerRegister);
    qmlRegisterSingletonType<SettingsManager>("SettingsManager", 1, 0, "SettingsManager", &settingsManager);
    qmlRegisterSingletonType<Util>("Util", 1, 0, "Util", &utilRegister);
    qmlRegisterType<Waterfall>("Waterfall", 1, 0, "Waterfall");
    qmlRegisterType<Ping>("Ping", 1, 0, "Ping");
    qmlRegisterType<AbstractLink>("AbstractLink", 1, 0, "AbstractLink");

    qmlRegisterUncreatableMetaObject(
        Ping1DNamespace::staticMetaObject, "Ping1DNamespace", 1, 0, "Ping1DNamespace", "This is a enum."
    );

    QApplication app(argc, argv);

    // DPI support and HiDPI pixmaps
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QQmlApplicationEngine engine;

    // Load the QML and set the Context
    // Logo
#ifdef QT_NO_DEBUG
    engine.load(QUrl(QStringLiteral("qrc:/Logo.qml")));
    app.exec();
#endif
    // Main app
    QString gitUserRepo;
    //https://github.com/user/repo.git
    QRegularExpression regex("github.com/(.*).git");
    QRegularExpressionMatch match = regex.match(QStringLiteral(GIT_URL));
    if (match.hasMatch()) {
        gitUserRepo = match.capturedTexts()[1];
    }

    engine.rootContext()->setContextProperty("GitVersion", QStringLiteral(GIT_VERSION));
    engine.rootContext()->setContextProperty("GitVersionDate", QStringLiteral(GIT_VERSION_DATE));
    engine.rootContext()->setContextProperty("GitTag", QStringLiteral(GIT_TAG));
    engine.rootContext()->setContextProperty("GitUrl", QStringLiteral(GIT_URL));
    engine.rootContext()->setContextProperty("GitUserRepo", gitUserRepo);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    Logger::installHandler();
    qCInfo(mainCategory) << "Git version:" << GIT_VERSION;
    qCInfo(mainCategory) << "Git version date:" << GIT_VERSION_DATE;
    qCInfo(mainCategory) << "Git tag:" << GIT_TAG;
    qCInfo(mainCategory) << "Git url:" << GIT_URL;
    qCInfo(mainCategory) << "Git short origin:" << gitUserRepo;
    return app.exec();
}
