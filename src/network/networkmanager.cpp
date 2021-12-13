#include "networkmanager.h"
#include "logger.h"

#include <QDebug>
#include <QHostAddress>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkInterface>
#include <QNetworkReply>
#include <QQmlEngine>
#include <QTemporaryFile>

PING_LOGGING_CATEGORY(NETWORKMANAGER, "ping.networkmanager")

NetworkManager::NetworkManager() { QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership); }

void NetworkManager::requestJson(const QUrl& url, std::function<void(QJsonDocument&)> function)
{
    // Create manager and request
    auto manager = new QNetworkAccessManager;
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    // Take care of manager
    connect(manager, &QNetworkAccessManager::finished, self(), [function](QNetworkReply* reply) {
        reply->manager()->deleteLater();
        if (!reply) {
            qCWarning(NETWORKMANAGER) << "Reply is invalid.";
            return;
        }
        // Create QJsonDocument
        auto doc = QJsonDocument::fromJson(reply->readAll());
        function(doc);
    });

    manager->get(request);
}

void NetworkManager::download(const QUrl& url, std::function<void(QString)> function)
{
    // Create manager and request
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::PreferCache);

    // downloadManager will be deleted by `downloadManager->deleteLater();`
    auto downloadManager = new QNetworkAccessManager;
    auto reply = downloadManager->get(request);
    connect(reply, &QNetworkReply::downloadProgress, self(), [url](qint64 received, qint64 total) {
        if (total) {
            float percent = -1;
            if (total > 0) {
                percent = 100 * received / total;
            }
            qCDebug(NETWORKMANAGER)
                << QStringLiteral("Download [%1] %2 (%3KB)").arg(url.toString()).arg(percent).arg(received >> 10);
        }
    });
    connect(reply, &QNetworkReply::finished, self(), [downloadManager, function, reply, url] {
        qCDebug(NETWORKMANAGER) << QStringLiteral("Download %1 finished").arg(url.toString());
        // Save it to a file
        QTemporaryFile temporaryFile;
        bool ok = temporaryFile.open();
        auto byteArray = reply->readAll();
        downloadManager->deleteLater();
        if (!ok) {
            qCWarning(NETWORKMANAGER) << QStringLiteral("Was not possible to create temporary file to save "
                                                        "download content. Error: %s")
                                             .arg(temporaryFile.error());
            return;
        }
        temporaryFile.write(byteArray);
        temporaryFile.close();
        temporaryFile.setAutoRemove(false);
        function(temporaryFile.fileName());
    });
}

bool NetworkManager::isIpInSubnet(const QString& ip)
{
    const QHostAddress testAddress {ip};
    if (testAddress.protocol() == QAbstractSocket::IPv6Protocol) {
        qCWarning(NETWORKMANAGER) << "Invalid network interface for ip:" << testAddress;
        return false;
    }

    for (const auto& address : QNetworkInterface::allAddresses()) {
        // Remove the last value of the IP address
        const bool sameSubnet = (address.toIPv4Address() & 0xffffff00) == (testAddress.toIPv4Address() & 0xffffff00);
        qCDebug(NETWORKMANAGER)
            << QStringLiteral("Checking host address %1 against %2").arg(address.toString(), testAddress.toString());
        if (sameSubnet) {
            return true;
        }
    }

    return false;
}

bool NetworkManager::isIpSubnetBroadcast(const QString& ip)
{
    if (!NetworkManager::isIpInSubnet(ip)) {
        qCWarning(NETWORKMANAGER) << "IP address does not have a valid subnet:" << ip;
        return false;
    }

    return ip.endsWith("255");
}

QObject* NetworkManager::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}

NetworkManager* NetworkManager::self()
{
    static NetworkManager self;
    return &self;
}
