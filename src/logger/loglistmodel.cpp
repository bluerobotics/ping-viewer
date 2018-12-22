#include "loglistmodel.h"

LogListModel::LogListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    for(const auto& key : _roleNames.keys()) {
        _roles.append(key);
        _vectors.insert(key, {});
    }

    _filter.setSourceModel(this);
    _filter.setFilterRole(Roles::Visibility);
    _filter.setFilterWildcard("true");

    /**
     * @brief New logs should use append function, and this model will use the main eventloop to handle
     * the multithread problem via signal/emit
     */
    connect(this, &LogListModel::append, this, &LogListModel::doAppend);
}

QVariant LogListModel::data(const QModelIndex& index, int role) const
{
    const int indexRow = index.row();
    QVector<QVariant> vectorRole = _vectors[role];
    if(indexRow < 0 || vectorRole.size() <= indexRow) {
        return {"No valid data"};
    }
    return _vectors[role][indexRow];
}

void LogListModel::doAppend(const QString& time, const QString& text, const QColor& color, int category)
{
    bool visible = category & _categories;
    const int line = rowCount();
    beginInsertRows(QModelIndex(), line, line);
    _vectors[LogListModel::Time].append(time);
    _vectors[LogListModel::Display].append(text);
    _vectors[LogListModel::Foreground].append(color);
    _vectors[LogListModel::Category].append(category);
    _vectors[LogListModel::Visibility].append(visible);
    _size++;
    const auto& indexRow = index(line);
    endInsertRows();
    emit dataChanged(indexRow, indexRow, _roles);
    emit countChanged();
}

Q_INVOKABLE void LogListModel::filter(int categories)
{
    if(_categories == categories) {
        return;
    }

    _categories = categories;
    int i = 0;
    for(auto& rowCategory : _vectors[LogListModel::Category]) {
        bool visibility = rowCategory.toInt() & categories;
        if(visibility != _vectors[LogListModel::Visibility][i]) {
            _vectors[LogListModel::Visibility][i] = visibility;
            auto thisIndex = index(i);
            emit dataChanged(thisIndex, thisIndex, _roles);
        }
        i++;
    }
}

QHash<int, QByteArray> LogListModel::roleNames() const
{
    return _roleNames;
}
