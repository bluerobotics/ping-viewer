#pragma once

#include <QAbstractListModel>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

class QFile;
class QIODevice;

/**
 * @brief Abstract class to deal with generic settings
 *
 */
class QJsonSettings : public QAbstractListModel {
    Q_OBJECT
public:
    /**
     * @brief Construct a new QJsonSettings object
     *
     * @param parent
     */
    explicit QJsonSettings(QObject* parent = nullptr);

    /**
     * @brief Construct a new QJsonSettings object
     *
     * @param fileName
     * @param parent
     */
    QJsonSettings(const QString& fileName, QObject* parent = nullptr);

    /**
     * @brief Construct a new QJsonSettings object
     *
     * @param device
     * @param parent
     */
    QJsonSettings(QIODevice* device, QObject* parent = nullptr);

    /**
     * @brief Construct a new QJsonSettings object
     *
     * @param json
     * @param parent
     */
    QJsonSettings(const QByteArray& json, QObject* parent = nullptr);

    /**
     * @brief Destroy the QJsonSettings object
     *
     */
    ~QJsonSettings();

    /**
     * @brief Load json from file
     *
     * @param fileName
     * @return success
     */
    bool load(const QString& fileName);

    /**
     * @brief Load json from device
     *
     * @param device
     * @return success
     */
    bool load(QIODevice* device);

    /**
     * @brief Load json from QByteArray
     *
     * @param json
     * @return success
     */
    bool loadJson(const QByteArray& json);

    /**
     * @brief Get model row number
     *
     * @param parent
     * @return int
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * @brief Get data from index and role
     *
     * @param index
     * @param role
     * @return QVariant
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    /**
     * @brief Get role names
     *
     * @return QHash<int, QByteArray>
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Get json object from index
     *
     * @param index
     * @return Q_INVOKABLE object
     */
    Q_INVOKABLE QJsonObject object(int index) const;

    /**
     * @brief Get json object from key
     *
     * @param key
     * @return Q_INVOKABLE object
     */
    Q_INVOKABLE QJsonObject object(const QString& key) const;

private:
    Q_DISABLE_COPY(QJsonSettings)
    QJsonDocument _jsonDocument;
    QString _mainKey {QStringLiteral("settings")};
    QHash<int, QByteArray> _roles {
        {{0}, {"name"}},
        {{1}, {"type"}},
        {{2}, {"description"}},
    };
    QJsonValue _rootValue;
};
