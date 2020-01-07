#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickStyle>
#include <QDebug>
#include <QRegularExpression>

#if defined(QT_DEBUG) && defined(Q_OS_WIN)
    #include <KCrash>
#endif

#include "abstractlink.h"
#include "commandlineparser.h"
#include "devicemanager.h"
#include "filemanager.h"
#include "flasher.h"
#include "gradientscale.h"
#include "linkconfiguration.h"
#include "logger.h"
#include "notificationmanager.h"
#include "ping.h"
#include "ping360.h"
#include "polarplot.h"
#include "qquickopengl.h"
#include "settingsmanager.h"
#include "stylemanager.h"
#include "util.h"
#include "waterfallplot.h"

#include "qquickopengl.h"

Q_DECLARE_LOGGING_CATEGORY(mainCategory)

PING_LOGGING_CATEGORY(mainCategory, "ping.main")

int main(int argc, char *argv[])
{
    // Start logger ASAP
    Logger::installHandler();

    QCoreApplication::setOrganizationName("Blue Robotics Inc.");
    QCoreApplication::setOrganizationDomain("bluerobotics.com");
    QCoreApplication::setApplicationName("Ping Viewer");
    QCoreApplication::setApplicationVersion(GIT_TAG "-" GIT_VERSION "-" GIT_VERSION_DATE);

    QQuickStyle::setStyle("Material");

    // Singleton register
    qRegisterMetaType<AbstractLinkNamespace::LinkType>();
    qRegisterMetaType<PingEnumNamespace::PingDeviceType>();
    qRegisterMetaType<PingEnumNamespace::PingMessageId>();

    qmlRegisterSingletonType<DeviceManager>("DeviceManager", 1, 0, "DeviceManager",
                                            DeviceManager::qmlSingletonRegister);
    qmlRegisterSingletonType<FileManager>("FileManager", 1, 0, "FileManager", FileManager::qmlSingletonRegister);
    qmlRegisterSingletonType<Logger>("Logger", 1, 0, "Logger", Logger::qmlSingletonRegister);
    qmlRegisterSingletonType<NotificationManager>("NotificationManager", 1, 0, "NotificationManager",
            NotificationManager::qmlSingletonRegister);
    qmlRegisterSingletonType<SettingsManager>("SettingsManager", 1, 0, "SettingsManager",
            SettingsManager::qmlSingletonRegister);
    qmlRegisterSingletonType<StyleManager>("StyleManager", 1, 0, "StyleManager", StyleManager::qmlSingletonRegister);
    qmlRegisterSingletonType<Util>("Util", 1, 0, "Util", Util::qmlSingletonRegister);

    // Normal register
    qmlRegisterUncreatableType<AbstractLink>("AbstractLink", 1, 0, "AbstractLink",
            "Link abstraction class can't be created.");
    qmlRegisterType<Flasher>("Flasher", 1, 0, "Flasher");
    qmlRegisterType<GradientScale>("GradientScale", 1, 0, "GradientScale");
    qmlRegisterType<LinkConfiguration>("LinkConfiguration", 1, 0, "LinkConfiguration");
    qmlRegisterType<Ping>("Ping", 1, 0, "Ping");
    qmlRegisterType<Ping360>("Ping360", 1, 0, "Ping360");
    qmlRegisterType<PolarPlot>("PolarPlot", 1, 0, "PolarPlot");
    qmlRegisterType<QQuickOpenGL>("QQuickOpenGL", 1, 0, "QQuickOpenGL");
    qmlRegisterType<WaterfallPlot>("WaterfallPlot", 1, 0, "WaterfallPlot");

    qmlRegisterUncreatableMetaObject(
        AbstractLinkNamespace::staticMetaObject,
        "AbstractLinkNamespace", 1, 0, "AbstractLinkNamespace", "Namespace for LinkType enum access from QML."
    );

    qmlRegisterUncreatableMetaObject(
        PingEnumNamespace::staticMetaObject,
        "PingEnumNamespace", 1, 0, "PingEnumNamespace", "Namespace for Ping protocol enums access from QML."
    );

    // DPI support and HiDPI pixmaps
    // Attributes must be set before QCoreApplication is created.
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

    QApplication app(argc, argv);

    CommandLineParser parser(app);

    QQmlApplicationEngine engine;

    // Load the QML and set the Context
    // Logo
#ifdef QT_NO_DEBUG
    engine.load(QUrl(QStringLiteral("qrc:/Logo.qml")));
    app.exec();
#endif

    // Function used in CI to test runtime errors
    // After 5 seconds, check if qml engine was loaded
#ifdef AUTO_KILL
    QTimer *timer = new QTimer();
    QObject::connect(timer, &QTimer::timeout, [&app, &engine]() {
        if(engine.rootObjects().isEmpty()) {
            printf("Application failed to load GUI!");
            app.exit(-1);
        } else {
            app.exit(0);
        }
    });
    timer->start(5000);
#endif

    engine.rootContext()->setContextProperty("GitVersion", QStringLiteral(GIT_VERSION));
    engine.rootContext()->setContextProperty("GitVersionDate", QStringLiteral(GIT_VERSION_DATE));
    engine.rootContext()->setContextProperty("GitTag", QStringLiteral(GIT_TAG));
    engine.rootContext()->setContextProperty("GitUrl", QStringLiteral(GIT_URL));
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    qCInfo(mainCategory) << "Git version:" << GIT_VERSION;
    qCInfo(mainCategory) << "Git version date:" << GIT_VERSION_DATE;
    qCInfo(mainCategory) << "Git tag:" << GIT_TAG;
    qCInfo(mainCategory) << "Git url:" << GIT_URL;

    StyleManager::self()->setQmlEngine(&engine);

#if defined(QT_DEBUG) && defined(Q_OS_WIN)
    // Start KCrash
    KCrash::initialize();
#endif

    return app.exec();
}
