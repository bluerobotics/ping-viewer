#include "qjsonsettings.h"

#include <QDebug>
#include <QFile>
#include <QIODevice>

QJsonSettings::QJsonSettings(QObject *parent)
    : QAbstractListModel(parent)
{
}

QJsonSettings::QJsonSettings(const QString& fileName, QObject *parent)
    : QAbstractListModel(parent)
{
    load(fileName);
}

QJsonSettings::QJsonSettings(QIODevice * device, QObject *parent)
    : QAbstractListModel(parent)
{
    load(device);
}

QJsonSettings::QJsonSettings(const QByteArray& json, QObject *parent)
    : QAbstractListModel(parent)
{
    loadJson(json);
}

QJsonSettings::~QJsonSettings() = default;

bool QJsonSettings::load(const QString &fileName)
{
    QFile file(fileName);
    bool success = false;
    if (file.open(QIODevice::ReadOnly)) {
        success = load(&file);
        file.close();
    }

    return success;
}

bool QJsonSettings::load(QIODevice *device)
{
    return loadJson(device->readAll());
}

bool QJsonSettings::loadJson(const QByteArray &json)
{
    _jsonDocument = QJsonDocument::fromJson(json);

    if(_jsonDocument.isNull()) {
        qCritical() << "Failed to load json settings!";
        qDebug().noquote() << json;
        return false;
    }

    //TODO: Remove old Qt version code when using new QGC version
#if QT_VERSION > QT_VERSION_CHECK(5, 10, 0)
    _rootValue = _jsonDocument[_mainKey];
#else
    _rootValue = _jsonDocument.object()[_mainKey];
#endif

    if(!_rootValue.isArray()) {
        qCritical() << "Settings key need to be an array!";
        return false;
    }
    return true;
}

int QJsonSettings::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _rootValue.toArray().size();
}

QVariant QJsonSettings::data(const QModelIndex &index, int role) const
{
    // Role will be used in future, right now only name is used
    Q_UNUSED(role)

#if QT_VERSION > QT_VERSION_CHECK(5, 10, 0)
    return _rootValue.toArray().at(index.row())[QString(_roles[0])].toVariant();
#else
    return _rootValue.toArray().at(index.row()).toObject()[QString(_roles[0])].toVariant();
#endif
}

QHash<int, QByteArray> QJsonSettings::roleNames() const
{
    return _roles;
}

QJsonObject QJsonSettings::object(int index) const
{
    if(index > _rootValue.toArray().size() || index < 0) {
        return QJsonObject();
    }
    return _rootValue.toArray().at(index).toObject();
}

QJsonObject QJsonSettings::object(QString& key) const
{
    int size = _rootValue.toArray().size();
    for(int i = 0; i < size; i++) {
#if QT_VERSION > QT_VERSION_CHECK(5, 10, 0)
        if(_rootValue.toArray().at(i)[QString(_roles[0])].toString() == key) {
#else
        if(_rootValue.toArray().at(i).toObject()[QString(_roles[0])].toString() == key) {
#endif
            return _rootValue.toArray().at(i).toObject();
        }
    }
    return QJsonObject();
}