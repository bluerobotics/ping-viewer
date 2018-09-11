#pragma once

#include "abstractlink.h"

class TCPLink : public AbstractLink
{
public:
    TCPLink(QObject* parent = nullptr);
    ~TCPLink();
};
