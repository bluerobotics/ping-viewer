#include "notificationmanager.h"
#include "logger.h"
#include "stylemanager.h"

#include <QDebug>

PING_LOGGING_CATEGORY(NOTIFICATIONMANAGER, "ping.notificationmanager")

NotificationManager::NotificationManager()
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    connect(&_model, &NotificationModel::dataChanged, this, &NotificationManager::modelUpdate);
}

void NotificationManager::create(const QString& text, QColor color, QString icon)
{
    _model.add(text, icon, color);
    qDebug(NOTIFICATIONMANAGER) << "New Notification:" << text;
}

QObject* NotificationManager::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}

NotificationManager* NotificationManager::self()
{
    static NotificationManager self;
    return &self;
}
