#include "networkmanager.h"
#include "logger.h"

#include <QDebug>
#include <QHostAddress>
#include <QHostInfo>
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

QHostAddress NetworkManager::addressToIp(const QString& address)
{
    const QHostAddress testAddress(address);

    // Check first if the address is a valid IPV4/6 and avoid calling QHostInfo::fromName
    if (testAddress.protocol() == QAbstractSocket::IPv4Protocol
        || testAddress.protocol() == QAbstractSocket::IPv6Protocol) {
        return testAddress;
    }

    // This function can freeze the interface if something is wrong with the OS mDNS interface
    return QHostInfo::fromName(address).addresses().first();
}

bool NetworkManager::isAddressInSubnet(const QString& address)
{
    const QHostAddress testAddress = addressToIp(address);

    if (testAddress.protocol() == QAbstractSocket::IPv6Protocol) {
        qCWarning(NETWORKMANAGER) << "Invalid network interface for ip:" << testAddress;
        return false;
    }

    for (const auto& interface : QNetworkInterface::allInterfaces()) {
        for (const auto& networkAddressEntry : interface.addressEntries()) {
            const auto address = networkAddressEntry.ip();
            const auto netmask = networkAddressEntry.netmask();

            // Remove the last value of the IP address
            const bool sameSubnet = (address.toIPv4Address() & netmask.toIPv4Address())
                == (testAddress.toIPv4Address() & netmask.toIPv4Address());
            qCDebug(NETWORKMANAGER) << QStringLiteral(
                "Checking interface %1 (%2): host address %3 against %4 using netmask %5")
                                           .arg(interface.humanReadableName(), interface.name(), address.toString(),
                                               testAddress.toString(), netmask.toString());
            if (sameSubnet) {
                return true;
            }
        }
    }

    return false;
}

bool NetworkManager::isAddressSubnetBroadcast(const QString& address)
{
    const QString testAddress = addressToIp(address).toString();

    if (!NetworkManager::isAddressInSubnet(testAddress)) {
        qCWarning(NETWORKMANAGER) << "IP address does not have a valid subnet:" << testAddress;
        return false;
    }

    return testAddress.endsWith("255");
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
