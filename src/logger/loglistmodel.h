#pragma once

#include <math.h>

#include <QColor>
#include <QAbstractListModel>

#include <QSortFilterProxyModel>

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

    /**
     * @brief Model of visible items
     *
     * @return QSortFilterProxyModel*
     */
    QSortFilterProxyModel* filteredModel() { return &_filter; };
    Q_PROPERTY(QSortFilterProxyModel* filteredModel READ filteredModel CONSTANT)

signals:
    /**
     * @brief Append a new log message in model
     *  This is a signal that will do a trig doAppend
     *
     * @param time
     * @param text
     * @param color
     * @param category
     */
    void append(const QString& time, const QString& text, const QColor& color, int category);

    void countChanged();

private:
    Q_DISABLE_COPY(LogListModel)
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

    /**
     * @brief Append a new log message in model
     *  This message is used to avoid calls of begin/end insertRow from a second thread.
     *  The connection between append and doAppend will be done by the eventloop,
     *  avoiding any problem related to multthread calls, and as consequence
     *  executing doAppend and the begin/end row insertion signals in the main thread.
     *
     * @param time
     * @param text
     * @param color
     * @param category
     */
    void doAppend(const QString& time, const QString& text, const QColor& color, int category);

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

    QSortFilterProxyModel _filter;
};

Q_DECLARE_METATYPE(LogListModel*)
Q_DECLARE_METATYPE(QSortFilterProxyModel*)
