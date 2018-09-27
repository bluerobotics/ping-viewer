#pragma once

#include "abstractlink.h"

/**
 * @brief TCP connection class
 *
 */
class TCPLink : public AbstractLink
{
public:
    /**
     * @brief Construct a new TCPLink object
     *
     * @param parent
     */
    TCPLink(QObject* parent = nullptr);

    /**
     * @brief Destroy the TCPLink object
     *
     */
    ~TCPLink();
};
