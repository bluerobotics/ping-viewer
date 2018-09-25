#pragma once

#include <QVector>

/**
 * @brief Ring vector class template
 *
 * @tparam T
 */
template <typename T>
class RingVector: public QVector<T>
{
public:
    RingVector()
        : _accessType(FIFO)
        , _appendIndex(-1) {}

    /**
     * @brief Ring buffer type
     *
     */
    enum ACCESS_TYPE {
        FIFO,   // 0 will be the newest data to append
        LIFO    // 0 will be the oldest data to append
    };

    /**
     * @brief Vector access type
     */
    void setAccessType(ACCESS_TYPE accessType)
    {
        _accessType = accessType;
    }

    /**
     * @brief Access vector
     */
    T& operator[](int id)
    {
        int index;
        if(_accessType == FIFO) {
            index = _appendIndex - id;
            while(index < 0) {
                index += QVector<T>::length();
            }
        } else {
            index = id;
        }
        return QVector<T>::operator[](index%QVector<T>::length());
    }

    /**
     * @brief Append value in vector, remove oldest
     */
    void append(const T &value)
    {
        _appendIndex++;
        QVector<T>::operator[](_appendIndex%QVector<T>::length()) = value;
    }

private:
    ACCESS_TYPE _accessType;
    uint _appendIndex;
};