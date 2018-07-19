#include <QDateTime>
#include <QStandardPaths>

#include "filemanager.h"
#include "logger.h"

PING_LOGGING_CATEGORY(FILEMANAGER, "ping.filemanager");

FileManager::FileManager()
    : _docDir{.dir = QDir(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0])}
    , _fmDir{.dir = _docDir.dir.filePath(QStringLiteral("PingViewer"))}
    , _guiLogDir{.dir = _fmDir.dir.filePath(QStringLiteral("Gui_Log"))}
    , _picturesDir{.dir = _fmDir.dir.filePath(QStringLiteral("Pictures"))}
    , _sensorLogDir{.dir = _fmDir.dir.filePath(QStringLiteral("Sensor_Log"))}
{
    // Check for folders and create if necessary
    auto rootDir = QDir();
    for(auto f : {&_fmDir, &_guiLogDir, &_picturesDir, &_sensorLogDir}) {
        qCDebug(FILEMANAGER) << "Folder: " << f->dir;
        if(!f->dir.exists()) {
            qCDebug(FILEMANAGER) << "Create folder" << f->dir.path();
            f->ok = rootDir.mkpath(f->dir.path());
            qCDebug(FILEMANAGER) << (f->ok ? "Done." : ("Error while creating folder" + f->dir.path()));
            // Something is wrong, continue with the others
            if(!f->ok) {
                continue;
            }
        }

        // Everything is fine, but we need to make sure !
        f->ok = QFileInfo(f->dir.path()).isWritable();
    }
}

QObject* FileManager::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}

QString FileManager::createFileName(FileManager::FileType type)
{
    QString path = fileTypeFolder[type]->dir.path();
    QString result = path + "/" \
                     + QDateTime::currentDateTime().toString(_fileName) \
                     + fileTypeExtension[type];
    qCDebug(FILEMANAGER) << result;
    return result;
}

FileManager* FileManager::self()
{
    static FileManager* self = new FileManager();
    return self;
}

FileManager::~FileManager() = default;
