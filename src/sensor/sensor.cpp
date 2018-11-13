#include <QDateTime>
#include <QDebug>
#include <QLoggingCategory>

#include "filemanager.h"
#include "sensor.h"

#include "pingmessage/pingmessage.h"
#include "pingmessage/pingmessage_ping1D.h"

Q_LOGGING_CATEGORY(PING_PROTOCOL_SENSOR, "ping.protocol.sensor")

Sensor::Sensor() :
    _autodetect(true)
    ,_connected(false)
    ,_detector(new ProtocolDetector())
    ,_linkIn(new Link(LinkType::Serial, "Default"))
    ,_linkOut(nullptr)
    ,_parser(nullptr)
{
    emit connectionUpdate();
    connect(this, &Sensor::connectionOpen, this, [this] {
        this->_connected = true;
        emit this->connectionUpdate();
    });
    connect(this, &Sensor::connectionClose, this, [this] {
        this->_connected = false;
        emit this->connectionUpdate();
    });

    _detector->moveToThread(&_detectorThread);
    connect(&_detectorThread, &QThread::finished, _detector, &QObject::deleteLater);
    connect(&_detectorThread, &QThread::started, _detector, &ProtocolDetector::scan);
    connect(_detector, &ProtocolDetector::connectionDetected, this, [this](const LinkConfiguration& conConf) {
        connectLink(conConf);
    });
}

// TODO rework this after sublasses and parser rework
void Sensor::connectLink(const LinkConfiguration& conConf, const LinkConfiguration& logConf)
{
    if(_detector->isRunning()) {
        _detector->stop();
    }

    if(link()->isOpen()) {
        link()->finishConnection();
    }

    qCDebug(PING_PROTOCOL_SENSOR) << "Connecting to" << conConf;
    if(!conConf.isValid()) {
        qCWarning(PING_PROTOCOL_SENSOR) << LinkConfiguration::errorToString(conConf.error());
        return;
    }
    if(link()) {
        _linkIn.clear();
    }
    _linkIn = QSharedPointer<Link>(new Link(conConf));
    link()->startConnection();

    if(!link()->isOpen()) {
        qCCritical(PING_PROTOCOL_SENSOR) << "Connection fail !" << conConf << link()->errorString();;
        emit connectionClose();
        return;
    }

    emit linkUpdate();

    if (_parser) {
        connect(link(), &AbstractLink::newData, _parser, &Parser::parseBuffer);
    }

    emit connectionOpen();

    // Disable log if playing one
    // Only save last link configuration if it's not a log
    if(link()->type() == LinkType::File) {
        if(!linkLog()) {
            return;
        }

        if(linkLog()->isOpen()) {
            linkLog()->finishConnection();
            _linkOut.clear();
        }
    } else {
        if(!logConf.isValid()) {
            connectLinkLog({LinkType::File, {FileManager::self()->createFileName(FileManager::Folder::SensorLog), QStringLiteral("w")}});
        } else {
            connectLinkLog(logConf);
        }
    }
}

void Sensor::connectLinkLog(const LinkConfiguration& logConf)
{
    if(linkLog()) {
        if(!linkLog()->isOpen()) {
            qCCritical(PING_PROTOCOL_SENSOR) << "No connection to log !" << linkLog()->errorString();
            return;
        }
        _linkOut.clear();
    }

    if(!logConf.isValid()) {
        qCWarning(PING_PROTOCOL_SENSOR) << LinkConfiguration::errorToString(logConf.error());
        return;
    }

    _linkOut = QSharedPointer<Link>(new Link(logConf));
    linkLog()->startConnection();

    if(!linkLog()->isOpen()) {
        qCCritical(PING_PROTOCOL_SENSOR) << "Connection with log fail !" << logConf << linkLog()->errorString();
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
    _detector->stop();
    _detectorThread.quit();
    _detectorThread.wait();
}
