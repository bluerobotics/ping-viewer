#pragma once

#include <QColor>
#include <QStringListModel>

/**
 * @brief Model for qml log interface
 * Initially based on:
 * https://stackoverflow.com/questions/37781426/how-to-change-the-color-of-qstringlistmodel-items
 *
 */
class LogListModel : public QStringListModel
{
public:
    /**
     * @brief Construct a new LogListModel
     *
     * @param parent
     */
    LogListModel(QObject* parent = nullptr);

    enum { TimeRole = Qt::UserRole + 0x10 };

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
     * @param index
     * @param value
     * @param role
     * @return true
     * @return false
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    /**
     * @brief Get role names
     *
     * @return QHash<int, QByteArray>
     */
    QHash<int, QByteArray> roleNames() const override;

private:
    QVector<int> _roles{
        Qt::ForegroundRole,
        LogListModel::TimeRole,
    };
    QHash<int, QByteArray> _roleNames{
        {{Qt::ForegroundRole}, {"foreground"}},
        {{LogListModel::TimeRole}, {"time"}},
    };
    QVector<QVariant> _rowColors;
    QVector<QVariant> _rowTimes;
};
