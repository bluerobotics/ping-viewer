#pragma once

#include <QtGlobal>

/** Calls the correct operator == in the P_PROPERTY macro */
template<typename T> bool pingIsSame(const T& a, const T& b)
{
    if constexpr (std::is_floating_point<T>::value) {
        return qFuzzyCompare(a, b);
    }

    return a == b;
}

#define P_PROPERTY(TYPE, NAME, VALUE) \
    Q_PROPERTY(TYPE NAME READ NAME WRITE NAME NOTIFY NAME##Changed) \
public: \
    TYPE NAME() { return _##NAME; } \
    void NAME(TYPE NAME) { \
        if(pingIsSame(NAME, _##NAME)) return; \
        _##NAME = NAME; emit NAME##Changed(); \
    } \
    Q_SIGNAL void NAME##Changed();\
private: \
    TYPE _##NAME = VALUE;
