#include <QDateTime>
#include <QQmlEngine>
#include <QStandardPaths>

#include "filemanager.h"
#include "logger.h"

PING_LOGGING_CATEGORY(FILEMANAGER, "ping.filemanager");

FileManager::FileManager()
    : _docDir{QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0]}
    , _fmDir{_docDir.dir.filePath(QStringLiteral("PingViewer"))}
    , _gradientsDir{_fmDir.dir.filePath(QStringLiteral("Waterfall_Gradients")), fileTypeExtension[TXT]}
    , _guiLogDir{_fmDir.dir.filePath(QStringLiteral("Gui_Log")), fileTypeExtension[TXT]}
    , _picturesDir{_fmDir.dir.filePath(QStringLiteral("Pictures")), fileTypeExtension[PICTURE]}
    , _sensorLogDir{_fmDir.dir.filePath(QStringLiteral("Sensor_Log")), fileTypeExtension[BINARY]}
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // Check for folders and create if necessary
    auto rootDir = QDir();
    for(auto f : {&_fmDir, &_guiLogDir, &_picturesDir, &_sensorLogDir, &_gradientsDir}) {
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

QFileInfoList FileManager::getFilesFrom(Folder folderType)
{
    FolderStruct* folder = folderMap[folderType];
    if(!folder) {
        qCWarning(FILEMANAGER) << "Folder pointer does not exist.";
        return {};
    }
    if(!folder->ok) {
        return {};
    }
    folder->dir.setSorting(QDir::Name);
    folder->dir.setFilter(QDir::Files);
    return folder->dir.entryInfoList();
}

QUrl FileManager::getPathFrom(FileManager::Folder folderType)
{
    FolderStruct* folder = folderMap[folderType];
    if(!folder) {
        qCWarning(FILEMANAGER) << "Folder pointer does not exist!";
        return {};
    }
    return QUrl::fromLocalFile(folder->dir.path());
}

QObject* FileManager::qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return self();
}

QString FileManager::createFileName(FileManager::Folder folderType)
{
    FolderStruct* folder = folderMap[folderType];
    if(!folder) {
        qCWarning(FILEMANAGER) << "Folder pointer does not exist!";
        return {};
    }
    const QString path = folder->dir.path();
    const QString result = path + "/" \
                     + QDateTime::currentDateTime().toString(_fileName) \
                     + folder->extension;
    qCDebug(FILEMANAGER) << "Creating file name:" << result;
    return result;
}

FileManager* FileManager::self()
{
    static FileManager* self = new FileManager();
    return self;
}

FileManager::~FileManager() = default;
