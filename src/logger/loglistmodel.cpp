#include "loglistmodel.h"

LogListModel::LogListModel(QObject* parent)
    : QStringListModel(parent)
{
    _roleNames.unite(QStringListModel::roleNames());
}

QVariant LogListModel::data(const QModelIndex& index, int role) const
{
    const int indexRow = index.row();
    switch(role) {
    case Qt::ForegroundRole : {
        if(indexRow > 0 && indexRow < _rowColors.size()) {
            return _rowColors[indexRow];
        }
    }
    break;
    case LogListModel::TimeRole : {
        if(indexRow > 0 && indexRow < _rowTimes.size()) {
            return _rowTimes[indexRow];
        }
    }
    break;
    default:
        break;
    }

    return QStringListModel::data(index, role);
}

bool LogListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    int indexRow = index.row();
    switch (role) {
    case Qt::ForegroundRole :
        if(indexRow > 0 && indexRow < _rowColors.size()) {
            _rowColors[indexRow] = value;
        } else if(indexRow == _rowColors.size()) {
            _rowColors.append(value);
        } else {
            return true;
        }
        emit dataChanged(index, index, _roles);
        return true;
    case LogListModel::TimeRole :
        if(indexRow > 0 && indexRow < _rowTimes.size()) {
            _rowTimes[indexRow] = value;
        } else if(indexRow == _rowTimes.size()) {
            _rowTimes.append(value);
        } else {
            return true;
        }
        emit dataChanged(index, index, _roles);
        return true;
    default :
        break;
    }

    return QStringListModel::setData(index, value, role);
}

QHash<int, QByteArray> LogListModel::roleNames() const
{
    return _roleNames;
}
