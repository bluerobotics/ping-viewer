#pragma once

#include <QColor>
#include <QAbstractListModel>

/**
 * @brief Notification model
 *
 */
class NotificationModel : public QAbstractListModel
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new NotificationModel
     *
     * @param parent
     */
    NotificationModel(QObject* parent = nullptr);

    enum {
        Text,
        Color,
        Time,
        Icon,
        Severity,
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
     * @brief Set the data
     *
     * @param index Model
     * @param value Value to set
     * @param role Model role
     * @return true Was able to set value
     * @return false Not able to set value
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    /**
     * @brief Get role names
     *
     * @return QHash<int, QByteArray>
     */
    QHash<int, QByteArray> roleNames() const override;

    /**
     * @brief Return the number of rows
     *
     * @param parent
     * @return int
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        Q_UNUSED(parent)
        return _size;
    }

    /**
     * @brief Inserts count rows into the model, beginning at the given row.
     *  This implementation was based over QStringListModel
     * @param row Row to be used
     * @param count Number of times
     * @param parent
     * @return true If everything goes fine
     * @return false It was not able to insert a new row
     */
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    /**
     * @brief Add a new notification in the model
     *
     * @param text
     * @param icon
     * @param color
     * @param severity
     */
    Q_INVOKABLE void add(
        QVariant text = QStringLiteral("This is a empty notification."),
        QVariant icon = {},
        QVariant color = QColor("DarkGray"),
        QVariant severity = {}
    );

    /**
     * @brief Remove index in model
     *
     * @param index
     */
    Q_INVOKABLE void remove(int index);

    /**
     * @brief Remove all items
     *
     */
    Q_INVOKABLE void removeAll();

private:
    Q_DISABLE_COPY(NotificationModel)
    QVector<int> _roles;
    int _size = 0;

    QHash<int, QByteArray> _roleNames{
        {{NotificationModel::Color}, {"foreground"}},
        {{NotificationModel::Icon}, {"icon"}},
        {{NotificationModel::Severity}, {"severity"}},
        {{NotificationModel::Time}, {"time"}},
        {{NotificationModel::Text}, {"display"}},
    };

    QVector<QVariant> _rowColor;
    QVector<QVariant> _rowIcon;
    QVector<QVariant> _rowSeverity;
    QVector<QVariant> _rowText;
    QVector<QVariant> _rowTime;

    QHash<int, QVector<QVariant>*> _rows {
        {NotificationModel::Color, &_rowColor},
        {NotificationModel::Icon, &_rowIcon},
        {NotificationModel::Severity, &_rowSeverity},
        {NotificationModel::Text, &_rowText},
        {NotificationModel::Time, &_rowTime},
    };
};
