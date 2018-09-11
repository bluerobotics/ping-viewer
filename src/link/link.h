#pragma once

#include <QObject>

#include "abstractlink.h"
#include "linkconfiguration.h"

class Link : public QObject
{
    Q_OBJECT
public:
    Link(LinkType linkType = LinkType::None, QString name = QString(), QObject* parent = nullptr);
    Link(const LinkConfiguration& linkConfiguration, QObject* parent = nullptr);
    ~Link();

    //This will handle the connection change
    AbstractLink* self() { return _abstractLink; };

private:
    AbstractLink* _abstractLink;
};