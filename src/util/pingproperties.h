#pragma once

#include <QtGlobal>

/** Calls the correct operator == in the P_PROPERTY macro */
template<typename T> bool pingIsSame(const T& a, const T& b)
{
    return a == b;
}

/** Calls the correct operator == in the P_PROPERTY macro */
template<> bool pingIsSame<float>(const float& a, const float &b);

/** Calls the correct operator == in the P_PROPERTY macro */
template<> bool pingIsSame<double>(const double& a, const double& b);

#define P_PROPERTY(TYPE, NAME, VALUE) \
    Q_PROPERTY(TYPE NAME READ NAME WRITE NAME NOTIFY NAME##Changed) \
public: \
    TYPE NAME() const { return _##NAME; } \
    void NAME(TYPE NAME) { \
        if(pingIsSame(NAME, _##NAME)) return; \
        _##NAME = NAME; emit NAME##Changed(); \
    } \
    Q_SIGNAL void NAME##Changed();\
private: \
    TYPE _##NAME = VALUE;
