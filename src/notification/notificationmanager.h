#pragma once

#include <QLoggingCategory>

#include "stylemanager.h"
#include "notificationmodel.h"

class QJSEngine;
class QQmlEngine;

Q_DECLARE_LOGGING_CATEGORY(NOTIFICATIONMANAGER)

/**
 * @brief Manage the project NotificationManager
 *
 */
class NotificationManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Return NotificationManager pointer
     *
     * @return NotificationManager*
     */
    static NotificationManager* self();

    /**
     * @brief Return a pointer of this singleton to the qml register function
     *
     * @param engine
     * @param scriptEngine
     * @return QObject*
     */
    static QObject* qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine);

    /**
     * @brief Get the log model
     *
     * @return NotificationModel*
     */
    NotificationModel* model() { return &_model; };
    Q_PROPERTY(NotificationModel* model READ model NOTIFY modelUpdate)

    /**
     * @brief Create a new notification
     *
     * @param text
     * @param color
     * @param icon
     */
    void create(const QString& text, QColor color, QString icon = StyleManager::chatIcon());

signals:
    void modelUpdate();

private:
    Q_DISABLE_COPY(NotificationManager)
    /**
     * @brief Construct a new Notification Manager object
     *
     */
    NotificationManager();

    NotificationModel _model;
};
