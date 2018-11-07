#include "logger.h"
#include "maddy/parser.h"
#include "networkmanager.h"
#include "networktool.h"
#include "notificationmanager.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QRegularExpression>
#include <QTimer>
#include <QUrl>

PING_LOGGING_CATEGORY(NETWORKTOOL, "ping.networktool")

void NetworkTool::checkInterfaceUpdate()
{
    static QString gitUserRepo;
    if(gitUserRepo.isEmpty()) {
        //*github.com/user/repo* results in user/repo
        const static QRegularExpression regex(R"(github.com\/([^.]*))");
        QRegularExpressionMatch regexMatch = regex.match(QStringLiteral(GIT_URL));
        // TODO: Check regex output
        if (regexMatch.hasMatch()) {
            gitUserRepo = regexMatch.capturedTexts()[1];
        } else {
            qCWarning(NETWORKTOOL) <<
                                   "Fail to get github user and repository! It'll not be possible to check for updates";
            return;
        }
    }

    const QUrl url{QStringLiteral("https://api.github.com/repos/%1/releases").arg(gitUserRepo)};
    NetworkManager::self()->requestJson(
        url,
        self()->checkNewVersionInGitHubPayload
    );
}

void NetworkTool::checkNewVersionInGitHubPayload(QJsonDocument& jsonDocument)
{
    const static QString projectTag = QStringLiteral(GIT_TAG);

    // Check if version is a normal release
    // For everything else we can presume test release
    static QRegularExpression releaseTagRegex(QStringLiteral(R"([v]\d+\.\d+)"));
    QRegularExpressionMatch regexMatch = releaseTagRegex.match(projectTag);
    // Check if this actual version is a real release (normal or test)
    // Alarm people to download the correction version !
    float actualVersion = regexMatch.hasMatch() ? projectTag.right(1).toFloat() : -1.0;

    // If this version does not follow vd+.d+, this is a test or continuous release
    if(regexMatch.hasMatch()) {
        qCDebug(NETWORKTOOL) << "Running release version:" << projectTag;
    } else {
        releaseTagRegex.setPattern(QStringLiteral(R"([v,t]\d+\.\d+)"));
        qCDebug(NETWORKTOOL) << "Running test version:" << projectTag;
    }

    auto versionsAvailable = jsonDocument.array();
    if(versionsAvailable.isEmpty()) {
        qCWarning(NETWORKTOOL) << "GitHub json it's not an array!";
        qCDebug(NETWORKTOOL) << jsonDocument;
        return;
    }

    struct {
        float version;
        QString versionString;
        QJsonValue json;
    } lastReleaseAvailable;
    lastReleaseAvailable.version = actualVersion;

    // Check payload
    for(const QJsonValue& versionPayload : versionsAvailable) {
        auto versionString = versionPayload[QStringLiteral("tag_name")].toString();
        qCDebug(NETWORKTOOL) << "Testing version:" << versionString;

        regexMatch = releaseTagRegex.match(versionString);
        if(!regexMatch.hasMatch()) {
            qCDebug(NETWORKTOOL) << "Not a release." << regexMatch;
            continue;
        }
        qCDebug(NETWORKTOOL) << "Possible new version.";

        auto version = versionString.right(1).toFloat();
        if(version > lastReleaseAvailable.version) {
            lastReleaseAvailable.version = version;
            lastReleaseAvailable.versionString = versionString;
            lastReleaseAvailable.json = versionPayload;
        }
    }

    // No new version available
    if(actualVersion == lastReleaseAvailable.version) {
        qCInfo(NETWORKTOOL) << "Already running last release version.";
        return;
    }

    if(actualVersion < 0) {
        NotificationManager::self()->create(
            "This isn't a release!\n\rPlease download a release version.", "red", StyleManager::reportIcon());
    }

    // Use the release url by default
    static QString downloadLink = lastReleaseAvailable.json["html_url"].toString();

    // Try to find the correct file to download
    auto assets = lastReleaseAvailable.json["assets"].toArray();
    if(!assets.isEmpty()) {

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

        for(const QJsonValue& artifact : assets) {
            if(artifact["name"].toString().contains(extension)) {
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

    QString newVersionText =
        QStringLiteral("New version: %1 - %2<br>%3").arg(
            lastReleaseAvailable.versionString,
            //TODO: Move the breakline feature to maddy after italic and bold PRs approved
            QString::fromStdString(parser->Parse(markdownInput)).remove("<p>").remove("</p>").replace("\r", "<br>"),
            QString("<a href=\"%1\">DOWNLOAD IT HERE!</a>").arg(downloadLink)
        );
    NotificationManager::self()->create(
        newVersionText, "green", StyleManager::infoIcon());
}

void NetworkTool::scheduleUpdateCheck()
{
    QTimer *timer = new QTimer(this);
    QObject::connect(timer, &QTimer::timeout, this, [this, timer] {
        checkInterfaceUpdate();
        timer->deleteLater();
    });
    timer->setSingleShot(true);
    timer->start(30000);
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
void _pingViewerUpdateCheck()
{
    NetworkTool::self()->scheduleUpdateCheck();
}
Q_COREAPP_STARTUP_FUNCTION(_pingViewerUpdateCheck);
