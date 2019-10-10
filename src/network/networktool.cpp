#include "networktool.h"
#include "logger.h"
#include "maddy/parser.h"
#include "networkmanager.h"
#include "notificationmanager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QTimer>
#include <QUrl>

PING_LOGGING_CATEGORY(NETWORKTOOL, "ping.networktool")

QString NetworkTool::_gitUserRepo = "bluerobotics/ping-viewer";
QString NetworkTool::_gitUserRepoFirmware = "bluerobotics/ping-firmware";

NetworkTool::NetworkTool()
{
    //*github.com/user/repo* results in user/repo
    const static QRegularExpression regex(R"(github.com\/([^.]*))");
    QRegularExpressionMatch regexMatch = regex.match(QStringLiteral(GIT_URL));
    if (!regexMatch.hasMatch() || regexMatch.lastCapturedIndex() < 1) {
        qCWarning(NETWORKTOOL) << "Fail to get github user and repository! "
                                  "It will not be possible to check for updates. "
                                  "Using default value:"
                               << _gitUserRepo;
        qCDebug(NETWORKTOOL) << "GIT_URL value:" << QStringLiteral(GIT_URL);
        return;
    }
    _gitUserRepo = regexMatch.capturedTexts()[1];
}

void NetworkTool::checkInterfaceUpdate()
{
    static const QUrl url {QStringLiteral("https://api.github.com/repos/%1/releases").arg(_gitUserRepo)};
    NetworkManager::self()->requestJson(url, self()->checkNewVersionInGitHubPayload);
}

void NetworkTool::checkNewVersionInGitHubPayload(const QJsonDocument& jsonDocument)
{
    const static QString projectTag = QStringLiteral(GIT_TAG);

    auto semverToInt = [](const QString& version) -> int {
        int major, minor, patch = 0;
        std::sscanf(version.toStdString().c_str(), "%d.%d.%d", &major, &minor, &patch);
        return (major << 24) + (minor << 16) + (patch << 8);
    };

    // Check if version is a normal release
    // For everything else we can presume test release
    // This regex will ONLY check for vX.X.X
    static QRegularExpression releaseTagRegex(QStringLiteral(R"([v]\d+\.\d+\.\d+)"));
    QRegularExpressionMatch regexMatch = releaseTagRegex.match(projectTag);
    // Check if this actual version is a real release (normal or test)
    // Alarm people to download the correction version !
    // strip leading character with .mid(1)
    int actualVersion = regexMatch.hasMatch() ? semverToInt(projectTag.mid(1)) : -1.0;

    // If this version does not follow vd+.d+.d+, this is a test or continuous release
    if (regexMatch.hasMatch()) {
        qCDebug(NETWORKTOOL) << "Running release version:" << projectTag << "#" << actualVersion;
    } else {
        releaseTagRegex.setPattern(QStringLiteral(R"([t,v]\d+\.\d+\.\d+)"));
        qCDebug(NETWORKTOOL) << "Running test version:" << projectTag;
    }

    auto versionsAvailable = jsonDocument.array();
    if (versionsAvailable.isEmpty()) {
        qCWarning(NETWORKTOOL) << "GitHub json it's not an array!";
        qCDebug(NETWORKTOOL) << jsonDocument;
        return;
    }

    struct {
        int version;
        QString versionString;
        QJsonValue json;
    } lastReleaseAvailable;
    lastReleaseAvailable.version = actualVersion;

    // Check payload
    for (const QJsonValue& versionPayload : versionsAvailable) {
        auto versionString = versionPayload[QStringLiteral("tag_name")].toString();
        qCDebug(NETWORKTOOL) << "Testing version:" << versionString;

        regexMatch = releaseTagRegex.match(versionString);
        if (!regexMatch.hasMatch()) {
            qCDebug(NETWORKTOOL) << "Not a release." << regexMatch;
            continue;
        }
        qCDebug(NETWORKTOOL) << "Possible new version.";

        // strip leading character with .mid(1)
        auto version = semverToInt(versionString.mid(1));
        qCDebug(NETWORKTOOL) << "comparing version:" << versionString << "#" << version;
        if (version > lastReleaseAvailable.version) {
            lastReleaseAvailable.version = version;
            lastReleaseAvailable.versionString = versionString;
            lastReleaseAvailable.json = versionPayload;
        }
    }

    // No new version available
    if (actualVersion == lastReleaseAvailable.version) {
        qCInfo(NETWORKTOOL) << "Already running last release version.";
        return;
    }

    // If it's running a test release and this is not the last available
    if (actualVersion < 0 && projectTag != lastReleaseAvailable.versionString) {
        NotificationManager::self()->create(
            "This isn't a release!\n\rPlease download a release version.", "red", StyleManager::reportIcon());
    }

    // Use the release url by default
    static QString downloadLink = lastReleaseAvailable.json["html_url"].toString();

    // Try to find the correct file to download
    auto assets = lastReleaseAvailable.json["assets"].toArray();
    if (!assets.isEmpty()) {

        // Astyle is not smart enough
        // *INDENT-OFF*
        static const QString extension =
#ifdef Q_OS_LINUX
            QStringLiteral("x86_64.AppImage");
#endif
#ifdef Q_OS_OSX
        QStringLiteral("release.dmg");
#else
            QStringLiteral("release.zip");
#endif
        // *INDENT-ON*

        for (const QJsonValue& artifact : assets) {
            if (artifact["name"].toString().contains(extension)) {
                downloadLink = artifact["browser_download_url"].toString();
                break;
            };
        }
    } else {
        qCWarning(NETWORKTOOL) << "Assets are empty!";
        qCDebug(NETWORKTOOL) << lastReleaseAvailable.json;
    }

    // Transform mk to html
    std::stringstream markdownInput(lastReleaseAvailable.json[QStringLiteral("body")].toString().toStdString());
    std::shared_ptr<maddy::Parser> parser = std::make_shared<maddy::Parser>();

    QString newVersionText = QStringLiteral("New version: %1 - %2<br>%3")
                                 .arg(lastReleaseAvailable.versionString,
                                     // TODO: Move the breakline feature to maddy after italic and bold PRs approved
                                     QString::fromStdString(parser->Parse(markdownInput))
                                         .remove("<p>")
                                         .remove("</p>")
                                         .replace("\r", "<br>"),
                                     QString("<a href=\"%1\">DOWNLOAD IT HERE!</a>").arg(downloadLink));
    NotificationManager::self()->create(newVersionText, "green", StyleManager::infoIcon());
}

void NetworkTool::scheduleUpdateCheck()
{
    QTimer* timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, [timer] {
        checkInterfaceUpdate();
        timer->deleteLater();
    });
    timer->setSingleShot(true);
    timer->start(30000);
}

void NetworkTool::checkNewFirmware(const QString& sensorName, std::function<void(QJsonDocument&)> function)
{
    static const QUrl url {
        QStringLiteral("https://api.github.com/repos/%1/contents/%2").arg(_gitUserRepoFirmware, sensorName)};
    NetworkManager::self()->requestJson(url, function);
}

QObject* NetworkTool::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}

NetworkTool* NetworkTool::self()
{
    static NetworkTool* self = new NetworkTool();
    return self;
}

// Start update check
void _pingViewerUpdateCheck() { NetworkTool::self()->scheduleUpdateCheck(); }
Q_COREAPP_STARTUP_FUNCTION(_pingViewerUpdateCheck);
