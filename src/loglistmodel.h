#pragma once

#include <QColor>
#include <QStringListModel>

/**
 * @brief Model for qml log interface
 * Thank you very much https://stackoverflow.com/questions/37781426/how-to-change-the-color-of-qstringlistmodel-items
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
    LogListModel(QObject* parent = nullptr)
        : QStringListModel(parent)
    {}

    enum { TimeRole = Qt::UserRole + 0x10 };

    /**
     * @brief Return data
     *
     * @param index
     * @param role
     * @return QVariant
     */
    QVariant data(const QModelIndex & index, int role) const override;

    /**
     * @brief Set the data
     *
     * @param index
     * @param value
     * @param role
     * @return true
     * @return false
     */
    bool setData(const QModelIndex & index, const QVariant & value, int role) override;

    /**
     * @brief Get role names
     *
     * @return QHash<int, QByteArray>
     */
    QHash<int, QByteArray> roleNames() const override;

private:
    std::map<int, QColor> _rowColors;
    std::map<int, QString> _rowTimes;
};