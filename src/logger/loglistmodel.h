#pragma once

#include <math.h>

#include <QColor>
#include <QAbstractListModel>

/**
 * @brief Model for qml log interface
 *
 */
class LogListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new LogListModel
     *
     * @param parent
     */
    LogListModel(QObject* parent = nullptr);

    /**
     * @brief Roles
     *
     */
    enum Roles {
        Category,
        Display,
        Foreground,
        Time,
        Visibility,
    };

    /**
     * @brief Append a new log message in model
     *
     * @param time
     * @param text
     * @param color
     * @param category
     */
    void append(const QString& time, const QString& text, const QColor& color, int category);

    /**
     * @brief Return data
     *
     * @param index
     * @param role
     * @return QVariant
     */
    QVariant data(const QModelIndex& index, int role) const override;

    /**
     * @brief Get role names
     *
     * @return QHash<int, QByteArray>
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Apply filter in model
     *
     * @param categories
     */
    Q_INVOKABLE void filter(int categories);

    /**
     * @brief Return model size
     *
     * @param parent
     * @return int
     */
    Q_INVOKABLE int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        Q_UNUSED(parent);
        return _size;
    };

signals:
    void countChanged();

private:
    Q_DISABLE_COPY(LogListModel)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

    int _categories = 0;
    QVector<int> _roles;
    QHash<int, QByteArray> _roleNames{
        {{LogListModel::Category}, {"category"}},
        {{LogListModel::Display}, {"display"}},
        {{LogListModel::Foreground}, {"foreground"}},
        {{LogListModel::Time}, {"time"}},
        {{LogListModel::Visibility}, {"visibity"}},
    };
    int _size = 0;
    QHash<int, QVector<QVariant>> _vectors;
};

Q_DECLARE_METATYPE(LogListModel*)
