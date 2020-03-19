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
class NetworkManager : public QObject {
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
     * @brief Download a file and save in a temporary one
     *
     * @param url File to be downloaded
     * @param function Callback supplied with tmeporary file path of downloaded file
     */
    static void download(const QUrl& url, std::function<void(QString)> function);

    /**
     * @brief Check if IP address is a valid IP of the host network interfaces
     *
     * @param ip
     * @return true
     * @return false
     */
    static bool isIpInSubnet(const QString& ip);

    /**
     * @brief Return a pointer of this singleton to the qml register function
     *
     * @param engine
     * @param scriptEngine
     * @return QObject*
     */
    static QObject* qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine);

private:
    Q_DISABLE_COPY(NetworkManager)
    /**
     * @brief Construct a new Network Manager object
     *
     */
    NetworkManager();
};
