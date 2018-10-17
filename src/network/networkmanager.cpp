#include "logger.h"
#include "networkmanager.h"

#include <QDebug>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

PING_LOGGING_CATEGORY(NETWORKMANAGER, "ping.networkmanager")

void NetworkManager::requestJson(const QUrl& url, std::function<void(QJsonDocument&)> function)
{
    // Create manager and request
    auto manager = new QNetworkAccessManager;
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Take care of manager
    connect(manager, &QNetworkAccessManager::finished, self(), [function](QNetworkReply* reply) {
        reply->manager()->deleteLater();
        if(!reply) {
            qCWarning(NETWORKMANAGER) << "Reply is invalid.";
            return;
        }
        // Create QJsonDocument
        auto doc = QJsonDocument::fromJson(reply->readAll());
        function(doc);
    });

    manager->get(request);
}


QObject* NetworkManager::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}

NetworkManager* NetworkManager::self()
{
    static NetworkManager* self = new NetworkManager();
    return self;
}
