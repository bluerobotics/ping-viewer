#include "settingsmanager.h"
#include "linkconfiguration.h"
#include "logger.h"

#include <QQmlEngine>

PING_LOGGING_CATEGORY(SETTINGSMANAGER, "ping.settingsmanager")

SettingsManager::SettingsManager()
    : _settings("Blue Robotics Inc.", "Ping Viewer")
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    if (_settings.contains("reset")) {
        bool reset = _settings.value("reset").toBool();
        if (reset) {
            _settings.clear();
        }
    }

    if (_settings.contains("settingsTree")) {
        _tree.setMap(_settings.value("settingsTree").toMap());
    }

    qRegisterMetaType<QJsonSettings*>("const QJsonSettings*");
    loadLinkConfigurations();
}

void SettingsManager::loadLinkConfigurations()
{
    if (!_settings.contains("lastLinkConfigurations")) {
        qCDebug(SETTINGSMANAGER) << QStringLiteral("No last linkconfigurations found in settings.");
        return;
    }

    const auto variantLastLinkConfigurations = _settings.value("lastLinkConfigurations").value<QVariantList>();
    for (const auto variant : variantLastLinkConfigurations) {
        _lastLinkConfigurations.append(variant.value<LinkConfiguration>());
    }
}

void SettingsManager::saveLinkConfigurations()
{
    // Remove duplicated before saving it
    _lastLinkConfigurations.erase(
        std::unique(_lastLinkConfigurations.begin(), _lastLinkConfigurations.end(),
            [](const auto first, const auto second) {
                return (first.argsAsConst() == second.argsAsConst() && first.type() == second.type());
            }),
        _lastLinkConfigurations.end());

    // Move to QVariantList before saving
    // This is necessary since QList<QDataStream operator> does not work as expected
    QVariantList variants;
    for (const auto linkConfiguration : _lastLinkConfigurations) {
        variants.append(QVariant::fromValue(linkConfiguration));
    }
    set("lastLinkConfigurations", variants);
}

QVariant SettingsManager::value(const QString& settingName) const
{
    // Check if settings for that exist and get it, otherwise return default (0);
    if (_settings.contains(settingName)) {
        return _settings.value(settingName).toInt();
    }

    qCWarning(SETTINGSMANAGER) << QStringLiteral("Settings for %2 does not exist.").arg(settingName);
    return 0;
}

void SettingsManager::set(const QString& settingName, const QVariant& value)
{
    // Check if our map of models does have anything about it
    if (!_settings.contains(settingName)) {
        qCDebug(SETTINGSMANAGER) << QStringLiteral("New value in %1:").arg(settingName) << value;
    } else {
        qCDebug(SETTINGSMANAGER) << QStringLiteral("In %1:").arg(settingName) << value;
    }
    _settings.setValue(settingName, value);
    _settings.sync();
}

QObject* SettingsManager::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}

SettingsManager* SettingsManager::self()
{
    static SettingsManager self;
    return &self;
}

SettingsManager::~SettingsManager()
{
    saveLinkConfigurations();
    _settings.sync();
}
