#include "loglistmodel.h"

QVariant LogListModel::data(const QModelIndex & index, int role) const
{
    switch (role) {
    case Qt::ForegroundRole : {
        auto itr = _rowColors.find(index.row());
        if (itr != _rowColors.end()) {
            return itr->second;
        }
    }
    break;
    case LogListModel::TimeRole : {
        auto itr = _rowTimes.find(index.row());
        if (itr != _rowTimes.end()) {
            return itr->second;
        }
    }
    break;
    default:
        break;
    }

    return QStringListModel::data(index, role);
}

bool LogListModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
    switch (role) {
    case (Qt::ForegroundRole) :
        _rowColors[index.row()] = value.value<QColor>();
        emit dataChanged(index, index, _roles);
        return true;
    case (LogListModel::TimeRole) :
        _rowTimes[index.row()] = value.toString();
        emit dataChanged(index, index, _roles);
        return true;
    default :
        break;
    }

    return QStringListModel::setData(index, value, role);
}

QHash<int, QByteArray> LogListModel::roleNames() const
{
    QHash<int, QByteArray> ret = QStringListModel::roleNames();
    ret.insert(Qt::ForegroundRole, "foreground");
    ret.insert(LogListModel::TimeRole, "time");
    return ret;
}