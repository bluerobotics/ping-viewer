#include <QDateTime>
#include <QDebug>
#include <QLoggingCategory>

#include "sensor.h"

#include "pingmessage/pingmessage.h"
#include "pingmessage/pingmessage_ping1D.h"

Q_LOGGING_CATEGORY(PING_PROTOCOL_SENSOR, "ping.protocol.sensor")

Sensor::Sensor() :
    _autodetect(true)
    ,_linkIn(new Link(AbstractLink::LinkType::Serial, "Default"))
    ,_linkOut(nullptr)
    ,_parser(nullptr)
{

}

// TODO rework this after sublasses and parser rework
void Sensor::connectLink(QStringList connString, const QStringList& logConnString)
{
    if(link()->isOpen()) {
        link()->finishConnection();
    }

    qCDebug(PING_PROTOCOL_SENSOR) << "connecting to" << connString;
    if(connString.length() != 3) {
        qCWarning(PING_PROTOCOL_SENSOR) << "wrong size !";
        return;
    }
    if(connString[0].toInt() <= 0 || connString[0].toInt() > 5) {
        qCWarning(PING_PROTOCOL_SENSOR) << "wrong arg !";
        return;
    }
    if(link()) {
        _linkIn.clear();
    }
    _linkIn = QSharedPointer<Link>(new Link((AbstractLink::LinkType)connString[0].toInt(), "Default"));
    connString.removeFirst();

    link()->setConfiguration(connString);
    link()->startConnection();

    if(!link()->isOpen()) {
        qCCritical(PING_PROTOCOL_SENSOR) << "Connection fail !" << connString << link()->errorString();;
        emit connectionClose();
        return;
    }

    emit linkUpdate();

    if (_parser) {
        connect(link(), &AbstractLink::newData, _parser, &Parser::parseBuffer);
    }

    emit connectionOpen();

    // Disable log if playing one
    if(link()->type() == AbstractLink::LinkType::File) {
        if(!linkLog()) {
            return;
        }

        if(linkLog()->isOpen()) {
            linkLog()->finishConnection();
            _linkOut.clear();
        }
    } else { // Start log, if not playing one
        if(logConnString.isEmpty()) {
            QString fileName = QStringLiteral("%1.%2").arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyyMMdd-hhmmsszzz")), "bin");
            QStringList config{QStringLiteral("1"), fileName, QStringLiteral("w")};
            connectLinkLog(config);
        } else {
            connectLinkLog(logConnString);
        }
    }
}

void Sensor::connectLinkLog(QStringList connString)
{
    if(linkLog()) {
        if(!linkLog()->isOpen()) {
            qCCritical(PING_PROTOCOL_SENSOR) << "No connection to log !" << linkLog()->errorString();
            return;
        }
        _linkOut.clear();
    }

    if(connString.length() != 3) {
        qCWarning(PING_PROTOCOL_SENSOR) << "wrong size !" << connString;
        return;
    }
    if(connString[0].toInt() <= 0 || connString[0].toInt() > 5) {
        qCWarning(PING_PROTOCOL_SENSOR) << "wrong arg !" << connString;
        return;
    }

    _linkOut = QSharedPointer<Link>(new Link((AbstractLink::LinkType)connString[0].toInt(), "Log"));
    connString.removeFirst();

    linkLog()->setConfiguration(connString);
    linkLog()->startConnection();

    if(!linkLog()->isOpen()) {
        qCCritical(PING_PROTOCOL_SENSOR) << "Connection with log fail !" << connString << linkLog()->errorString();
        return;
    }

    connect(link(), &AbstractLink::newData, linkLog(), &AbstractLink::sendData);
    emit linkLogUpdate();
}

void Sensor::setAutoDetect(bool autodetect)
{
    if(_autodetect == autodetect) {
        return;
    }
    _autodetect = autodetect;
    emit autoDetectUpdate(autodetect);
}

Sensor::~Sensor()
{
}
