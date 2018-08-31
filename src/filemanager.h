#pragma once

#include <QDir>
#include <QLoggingCategory>
#include <QMap>
#include <QObject>
#include <QStringList>

Q_DECLARE_LOGGING_CATEGORY(FILEMANAGER);

class QJSEngine;
class QQmlEngine;

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
        TXT,
        PICTURE,
        BINARY
    };
    Q_ENUM(FileType)

    /**
     * @brief Abstract folder names and struct access
     */
    enum Folder {
        Documents,
        Gradients,
        GuiLogs,
        Pictures,
        PingDocuments,
        SensorLog,
    };
    Q_ENUM(Folder)

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

    /**
     * @brief Get the files from folder
     *
     * @param folderType
     * @return QFileInfoList List of files in folder
     */
    QFileInfoList getFilesFrom(Folder folderType);

    /**
     * @brief Return a pointer of this singleton to the qml register function
     *
     * @param engine
     * @param scriptEngine
     * @return QObject*
     */
    static QObject* qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine);

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
    folder _gradientsDir;
    folder _guiLogDir;
    folder _picturesDir;
    folder _sensorLogDir;

    /**
     * @brief Manage all folders access
     */
    QMap<Folder, folder*> folderMap{
        {Documents, &_fmDir},
        {Gradients, &_gradientsDir},
        {GuiLogs, &_guiLogDir},
        {Pictures, &_picturesDir},
        {PingDocuments, &_docDir},
        {SensorLog, &_sensorLogDir}
    };

    /**
     * @brief Manage file extensions
     *
     */
    const QMap<FileType, QString> fileTypeExtension {
        {TXT, ".txt"}
        , {PICTURE, ".png"}
        , {BINARY, ".bin"}
    };

    /**
     * @brief Manage file directories
     *
     */
    const QMap<FileType, folder*> fileTypeFolder {
        {TXT, &_gradientsDir},
        {TXT, &_guiLogDir},
        {PICTURE, &_picturesDir},
        {BINARY, &_sensorLogDir},
    };
};