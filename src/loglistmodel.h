#pragma once

#include <QColor>
#include <QStringListModel>

// Thank you very much https://stackoverflow.com/questions/37781426/how-to-change-the-color-of-qstringlistmodel-items
class LogListModel : public QStringListModel
{
public:
    LogListModel(QObject* parent = nullptr)
        : QStringListModel(parent)
    {}

    enum { TimeRole = Qt::UserRole + 0x10 };

    QVariant data(const QModelIndex & index, int role) const override;
    bool setData(const QModelIndex & index, const QVariant & value, int role) override;
    QHash<int, QByteArray> roleNames() const override;

private:
    std::map<int, QColor> _rowColors;
    std::map<int, QString> _rowTimes;
};