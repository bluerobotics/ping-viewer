#pragma once

#include <QDebug>
#include <QVariant>

/**
 * @brief This is something to handle a json like structure
 *
 */
class VariantTree {
public:
    /**
     * @brief Get variant reference from path
     *
     * @param path
     * @return QVariant&
     */
    QVariant& get(QStringList path);

    /**
     * @brief Get map
     *
     * @param path
     * @return QVariantMap*
     */
    QVariantMap* getMap(const QStringList& path);

    /**
     * @brief Get a copy of map
     *
     * @return const QVariantMap
     */
    const QVariantMap map() const { return _map; }

    /**
     * @brief Set new map
     *
     * @param map
     */
    void setMap(QVariantMap map) { _map = map; }

private:
    QVariantMap _map;
};

QDebug operator<<(QDebug d, const VariantTree& other);
