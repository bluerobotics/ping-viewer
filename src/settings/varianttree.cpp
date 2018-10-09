#include "varianttree.h"

QVariant& VariantTree::get(QStringList path)
{
    if(path.length()) {
        QString last = path.last();
        path.removeLast();
        return ((*getMap(path))[last]);
    }
    static QVariant a{"Invalid Path"};
    return a;
}

QVariantMap* VariantTree::getMap(QStringList path)
{
    QVariantMap* finalMap = &_map;
    for(auto& key : path) {
        if(!finalMap->contains(key)) {
            finalMap->insert(key, QVariantMap());
        } else if((*finalMap)[key].type() != QVariant::Map) {
            finalMap->insert(key, QVariantMap());
        }
        finalMap = static_cast<QVariantMap*>((*finalMap)[key].data());
    }
    return finalMap;
}

QDebug operator<<(QDebug d, const VariantTree& other)
{
    d << other.map();
    return d;
}
