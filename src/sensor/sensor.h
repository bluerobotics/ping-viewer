#pragma once

#include "link.h"
#include "parsers/parser.h"

// TODO rename to Device?
class Sensor : public QObject
{
    Q_OBJECT
public:
    Sensor();
    ~Sensor();

    Q_PROPERTY(AbstractLink* link READ link NOTIFY linkUpdate)
    AbstractLink* link() { return _linkIn ? _linkIn->self() : nullptr; };

    Q_PROPERTY(AbstractLink* linkLog READ linkLog NOTIFY linkLogUpdate)
    AbstractLink* linkLog() { return _linkOut ? _linkOut->self() : nullptr; };

    Q_PROPERTY(QString name READ name NOTIFY nameUpdate)
    QString name() { return _name; };

    Q_INVOKABLE void connectLink(const QString& connString);
    Q_INVOKABLE void connectLinkLog(const QString& connString);

    Q_PROPERTY(bool autodetect READ autoDetect WRITE setAutoDetect NOTIFY autoDetectUpdate)
    void setAutoDetect(bool autodetect) {
        if(_autodetect == autodetect) {
            return;
        }
        _autodetect = autodetect;
        emit autoDetectUpdate(autodetect);
    };
    bool autoDetect() { return _autodetect; };

protected:
    bool _autodetect;

    Link* _linkIn;
    Link* _linkOut;
    Parser* _parser; // communication implementation

    QString _name; // TODO populate

signals:
    void autoDetectUpdate(bool autodetect);

    // In
    void connectionClose();
    void connectionOpen();
    void nameUpdate();
    void linkUpdate();

    // Out
    void linkLogUpdate();
};
