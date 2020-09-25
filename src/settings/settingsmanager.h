#pragma once

#include <QLoggingCategory>
#include <QSettings>
#include <QStringListModel>

#include "linkconfiguration.h"
#include "qjsonsettings.h"
#include "settingsmanagerhelper.h"
#include "varianttree.h"

class QJSEngine;
class QQmlEngine;

Q_DECLARE_LOGGING_CATEGORY(SETTINGSMANAGER)

/**
 * @brief Manage the project SettingsManager
 *
 */
class SettingsManager : public QObject {
    Q_OBJECT
public:
    /**
     * @brief Get value from path
     *
     * @param path
     * @return Q_INVOKABLE getMapValue
     */
    Q_INVOKABLE QVariant getMapValue(const QStringList& path) { return _tree.get(path); }

    /**
     * @brief Get variable value
     *
     * @param settingName
     * @return QVariant
     */
    Q_INVOKABLE QVariant value(const QString& settingName) const;

    /**
     * @brief Set variable value
     *
     * @param settingName
     * @param value
     */
    Q_INVOKABLE void set(const QString& settingName, const QVariant& value);

    /**
     * @brief Set map value from path
     *
     * @param path
     * @param value
     */
    Q_INVOKABLE void setMapValue(const QStringList& path, const QVariant& value)
    {
        _tree.get(path) = value;
        _settings.setValue("settingsTree", _tree.map());
    }

    /**
     * @brief Return QSettings reference
     *
     * @return QSettings&
     */
    QSettings& settings() { return _settings; };

    /**
     * @brief Return SettingsManager pointer
     *
     * @return SettingsManager*
     */
    static SettingsManager* self();
    ~SettingsManager();

    /**
     * @brief Return a pointer of this singleton to the qml register function
     *
     * @param engine
     * @param scriptEngine
     * @return QObject*
     */
    static QObject* qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine);

    /**
     * @brief Return a pointer to the link configuration vector
     *
     * @return QVector<LinkConfiguration>*
     */
    QVector<LinkConfiguration>* lastLinkConfigurations() { return &_lastLinkConfigurations; }

    /**
     * @brief Save link configuration settings
     *
     */
    void saveLinkConfigurations();

private:
    Q_DISABLE_COPY(SettingsManager)
    /**
     * @brief Construct a new Settings Manager object
     *
     */
    SettingsManager();

    /**
     * @brief Load link configurations from settings
     *
     */
    void loadLinkConfigurations();

    QVector<LinkConfiguration> _lastLinkConfigurations;
    QSettings _settings;
    VariantTree _tree;

    /**
     * @brief This will create all gets, sets, signals and private variables,
     */

    // Everything after this line should be AUTO_PROPERTY
    AUTO_PROPERTY(uint, applicationOpacityIndex, 0)
    AUTO_PROPERTY(bool, alwaysOnTop, false)
    AUTO_PROPERTY(bool, debugMode, false)
    AUTO_PROPERTY(uint, enabledCategories, 0)
    AUTO_PROPERTY(bool, logScrollLock, true)
    AUTO_PROPERTY(bool, realTimeReplay, true)
    AUTO_PROPERTY(bool, replayMenu, false)
    AUTO_PROPERTY(bool, reset, false)
    AUTO_PROPERTY(bool, darkTheme, false)
    AUTO_PROPERTY(bool, enableSensorAdvancedConfiguration, false)
    // AUTO_PROPERTY_MODEL(QString, adistanceUnits, QStringList, MODEL({"Metric", "Imperial"})) // Example
    AUTO_PROPERTY_JSONMODEL(distanceUnits, QByteArrayLiteral(R"({
            "settings": [
                {
                    "name": "Metric",
                    "distance": "m",
                    "distanceScalar": 1
                },
                {
                    "name": "Imperial",
                    "distance": "ft",
                    "distanceScalar": 3.280839895
                }
            ]
        })"))
};
