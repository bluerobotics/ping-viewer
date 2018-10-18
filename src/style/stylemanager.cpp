#include "logger.h"
#include "settingsmanager.h"
#include "stylemanager.h"

#include <QDebug>
#include <QGuiApplication>
#include <QTimer>
#include <QWidget>

PING_LOGGING_CATEGORY(STYLEMANAGER, "ping.stylemanager")

StyleManager::StyleManager()
{
    connect(this, &StyleManager::isDarkChanged, this, [&] {
        theme(isDark() ? Theme::Dark : Theme::Light);
        primaryColor(isDark() ? light : dark);
        secondaryColor(isDark() ? dark : light);
    });

    isDark(SettingsManager::self()->darkTheme());
}

QSize StyleManager::displaySize() const
{
    // Check if we have engine access
    if(!_engine) {
        qCDebug(STYLEMANAGER) << "Need to set engine.";
        return {};
    }
    // We only have one window
    auto window = _engine->rootObjects().first();
    if(!window) {
        qCDebug(STYLEMANAGER) << "Window pointer is not available.";
        return {};
    }
    return {window->property("width").toInt(), window->property("height").toInt()};
}

QObject* StyleManager::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}

StyleManager* StyleManager::self()
{
    static StyleManager* self = new StyleManager();
    return self;
}
