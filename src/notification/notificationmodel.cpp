#include <QDebug>
#include <QTime>

#include "notificationmodel.h"

NotificationModel::NotificationModel(QObject* parent)
    : QAbstractListModel(parent)
{
    // Add all roles in _roles
    for(const auto key : _roleNames.keys()) {
        _roles.append(key);
    }
}

void NotificationModel::add(QVariant text, QVariant icon, QVariant color, QVariant severity)
{
    // Create a new row
    const QString time = QTime::currentTime().toString(QStringLiteral("[hh:mm:ss]"));
    const int line = rowCount();
    beginInsertRows(QModelIndex(), line, line);
    insertRows(line, 1);
    const QModelIndex idx = index(line);
    /* This should be just *one* call. */
    setData(idx, time, NotificationModel::Time);
    setData(idx, text, NotificationModel::Text);
    setData(idx, color, NotificationModel::Color);
    setData(idx, severity, NotificationModel::Severity);
    setData(idx, icon, NotificationModel::Icon);
    endInsertRows();
}

bool NotificationModel::insertRows(int row, int count, const QModelIndex& parent)
{

    // Check for valid input
    if (count < 1 || row < 0 || row > rowCount(parent)) {
        return false;
    }

    // Add all request rows
    while(count-- > 0) {
        for(auto* vector : _rows) {
            vector->insert(row, "");
        }
    }

    // Update size
    _size = _rowText.size();
    return true;
}

QVariant NotificationModel::data(const QModelIndex& index, int role) const
{
    const int indexRow = index.row();

    // Check if we have what is requested
    if(_rows.contains(role)) {
        QVector<QVariant>* row = _rows[role];
        if(indexRow >= 0 && indexRow < row->size()) {
            return row->at(indexRow);
        }
    }

    return {"No valid role or index."};
}

void NotificationModel::remove(int index)
{
    if(index < 0 && index > _size) {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    for(auto* vector : _rows) {
        vector->remove(index);
    }
    _size = _rowText.size();
    endRemoveRows();
}

void NotificationModel::removeAll()
{
    while(_rowText.size()) {
        remove(_rowText.size()-1);
    }
}

bool NotificationModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    const int indexRow = index.row();

    // Check if we can receive
    if(!_rows.contains(role)) {
        return false;
    }
    // Add the new data if possible
    QVector<QVariant>* row = _rows[role];
    if(indexRow >= 0 && indexRow < row->size()) {
        row->replace(indexRow, value);
    } else if(indexRow == row->size()) {
        row->append(value);
    } else {
        return false;
    }

    if(NotificationModel::Icon == role) {
        _size = _rowText.size();
        emit dataChanged(index, index, _roles);
    }
    return true;
}

QHash<int, QByteArray> NotificationModel::roleNames() const
{
    return _roleNames;
}
