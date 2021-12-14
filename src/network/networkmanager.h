#pragma once

#include <functional>

#include <QHostAddress>
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
     * @brief Fetch address information
     *
     * @param address
     * @return QHostAddress
     */
    static QHostAddress addressToIp(const QString& address);

    /**
     * @brief Check if an address is a valid IP of the host network interfaces
     *
     * @param address
     * @return true
     * @return false
     */
    static bool isAddressInSubnet(const QString& address);

    /**
     * @brief Check if an address is a valid broadcast IP in a /24 network.
     *
     * @param address
     * @return true
     * @return false
     */
    static bool isAddressSubnetBroadcast(const QString& address);

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
