#pragma once

#include <functional>

#include <QLoggingCategory>

class QJSEngine;
class QQmlEngine;

Q_DECLARE_LOGGING_CATEGORY(NETWORKMANAGER)

/**
 * @brief Manage the project NetworkManager
 *
 */
class NetworkManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Return NetworkManager pointer
     *
     * @return NetworkManager*
     */
    static NetworkManager* self();

    /**
     * @brief Do json requests
     * @param url URL to be requested
     * @param function that will be called
     *
     */
    static void requestJson(const QUrl& url, std::function<void(QJsonDocument&)> function);

    /**
     * @brief Return a pointer of this singleton to the qml register function
     *
     * @param engine
     * @param scriptEngine
     * @return QObject*
     */
    static QObject* qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine);

private:
    NetworkManager* operator = (NetworkManager& other) = delete;
    NetworkManager(const NetworkManager& other) = delete;
    NetworkManager() = default;
};
