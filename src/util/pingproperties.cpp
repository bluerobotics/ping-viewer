#include "pingproperties.h"


template<> inline bool pingIsSame<float>(const float& a, const float &b)
{
    return qFuzzyCompare(a, b);
}

template<> inline bool pingIsSame<double>(const double& a, const double& b)
{
    return qFuzzyCompare(a, b);
}
