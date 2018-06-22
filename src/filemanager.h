#pragma once

#include <QDir>
#include <QLoggingCategory>
#include <QMap>
#include <QObject>
#include <QStringList>

Q_DECLARE_LOGGING_CATEGORY(FILEMANAGER);

/**
 * @brief Manage folder creation, structure and filenames
 *
 */
class FileManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Manage file types
     */
    enum FileType {
        LOG,
        PICTURE,
        BINARY
    };
    Q_ENUM(FileType)

    /**
     * @brief Return FileManager singleton pointer
     *
     * @return FileManager*
     */
    static FileManager* self();
    ~FileManager();

    /**
     * @brief Return filename from type of file
     *
     * @param type
     * @return QString
     */
    Q_INVOKABLE QString createFileName(FileManager::FileType type);

private:
    /**
     * @brief Construct a new File Manager object
     *
     */
    FileManager();
    FileManager(const FileManager& other) = delete;
    FileManager* operator = (FileManager& other) = delete;

    /**
     * @brief Filename structure
     */
    const QString _fileName{QStringLiteral("yyyyMMdd-hhmmsszzz")};

    /**
     * @brief Structure to check folder existence
     *
     */
    struct folder {
        QDir dir;
        bool ok = false;
    };

    folder _docDir;
    folder _fmDir;
    folder _guiLogDir;
    folder _picturesDir;
    folder _sensorLogDir;

    /**
     * @brief Manage file extensions
     *
     */
    const QMap<FileType, QString> fileTypeExtension {
        {LOG, ".txt"}
        , {PICTURE, ".png"}
        , {BINARY, ".bin"}
    };

    /**
     * @brief Manage file directories
     *
     */
    const QMap<FileType, folder*> fileTypeFolder {
        {LOG, &_guiLogDir}
        , {PICTURE, &_picturesDir}
        , {BINARY, &_sensorLogDir}
    };
};