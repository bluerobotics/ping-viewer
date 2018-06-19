#pragma once

#include <QDir>
#include <QLoggingCategory>
#include <QMap>
#include <QObject>
#include <QStringList>

Q_DECLARE_LOGGING_CATEGORY(FILEMANAGER);

class FileManager : public QObject
{
    Q_OBJECT
public:
    enum FileType {
        LOG,
        PICTURE,
        BINARY
    };
    Q_ENUM(FileType)

    ~FileManager();
    static FileManager* self();

    Q_INVOKABLE QString createFileName(FileManager::FileType type);

private:
    FileManager* operator = (FileManager& other) = delete;
    FileManager(const FileManager& other) = delete;
    FileManager();

    const QString _fileName{QStringLiteral("yyyyMMdd-hhmmsszzz")};

    struct folder {
        QDir dir;
        bool ok = false;
    };

    folder _docDir;
    folder _fmDir;
    folder _guiLogDir;
    folder _picturesDir;
    folder _sensorLogDir;

    const QMap<FileType, QString> fileTypeExtension {
        {LOG, ".txt"}
        , {PICTURE, ".png"}
        , {BINARY, ".bin"}
    };

    const QMap<FileType, folder*> fileTypeFolder {
        {LOG, &_guiLogDir}
        , {PICTURE, &_picturesDir}
        , {BINARY, &_sensorLogDir}
    };

};