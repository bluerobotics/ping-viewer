#pragma once

#include <QLoggingCategory>
#include <QSettings>
#include <QStringListModel>

#include "settingsmanagerhelper.h"

Q_DECLARE_LOGGING_CATEGORY(SETTINGSMANAGER)

/**
 * @brief Manage the project SettingsManager
 *
 */
class SettingsManager : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief Get variable value
     *
     * @param settingName
     * @return QVariant
     */
    Q_INVOKABLE QVariant value(QString& settingName);

    /**
     * @brief Set variable value
     *
     * @param settingName
     * @param value
     */
    Q_INVOKABLE void set(QString& settingName, QVariant& value);

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
private:
    SettingsManager* operator = (SettingsManager& other) = delete;
    SettingsManager(const SettingsManager& other) = delete;
    SettingsManager();

    QSettings _settings;

    /**
     * @brief This will create all gets, sets, signals and private variables,
     */

    // Everything after this line should be AUTO_PROPERTY
    AUTO_PROPERTY(bool, debugMode)
    AUTO_PROPERTY(bool, replayMenu)
    AUTO_PROPERTY(bool, reset)
    AUTO_PROPERTY_MODEL(QString, distanceUnits, QStringList, MODEL({"Metric", "Imperial"}))
};