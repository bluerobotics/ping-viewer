#pragma once

#include <QDebug>
#include <QMap>
#include <QString>
#include <QStringList>

#include "abstractlinknamespace.h"

class LinkConfiguration : public QObject
{
    Q_OBJECT

public:
    struct LinkConf {
        QStringList args;
        QString name;
        LinkType type = LinkType::None;
    };

    enum Error {
        MissingConfiguration, // This can be used in future for warnings and not real errors
        NoErrors = 0,
        NoType,
        InvalidType,
        NoArgs,
        InvalidArgsNumber,
        ArgsAreEmpty,
    };

    static const QMap<Error, QString> errorMap;

    LinkConfiguration(LinkType linkType = LinkType::None, QStringList args = QStringList(), QString name = QString())
        : _linkConf{args, name, linkType} {};
    LinkConfiguration(LinkConf& confLinkStructure)
        : _linkConf{confLinkStructure} {};
    LinkConfiguration(const LinkConfiguration& other, QObject* parent = nullptr)
        : QObject(parent)
        , _linkConf{other.configurationStruct()} {};
    ~LinkConfiguration() = default;

    const QStringList* args() const { return &_linkConf.args; };
    Q_INVOKABLE QStringList argsAsConst() const { return _linkConf.args; };

    Q_INVOKABLE QString name() const { return _linkConf.name; };
    void setName(QString name) { _linkConf.name = name; };

    Q_INVOKABLE AbstractLinkNamespace::LinkType type() const { return _linkConf.type; };
    void setType(LinkType type) { _linkConf.type = type; };

    const QString createConfString() const { return _linkConf.args.join(":"); };
    const QStringList createConfStringList() const { return _linkConf.args; };

    const QString createFullConfString() const;
    const QStringList createFullConfStringList() const;

    LinkConf configurationStruct() const { return _linkConf; };
    const LinkConf* configurationStructPtr() const { return &_linkConf; };

    Q_INVOKABLE bool isValid() const { return error() <= NoErrors; }

    Error error() const;

    static QString errorToString(Error error) { return errorMap[error]; }

    LinkConfiguration& operator = (const LinkConfiguration& other)
    {
        this->_linkConf = other.configurationStruct();
        return *this;
    }

    bool checkType(LinkType type) { return _linkConf.type == type; };

    QString serialPort();
    int serialBaudrate();

    QString udpHost();
    int udpPort();

private:
    LinkConf _linkConf;
};

bool operator==(const LinkConfiguration& first, const LinkConfiguration& second);
QDebug operator<<(QDebug d, const LinkConfiguration& other);
QDataStream& operator<<(QDataStream &out, const LinkConfiguration linkConfiguration);
QDataStream& operator>>(QDataStream &in, LinkConfiguration &linkConfiguration);

Q_DECLARE_METATYPE(LinkConfiguration)

/**
 * @brief This allows us to register LinkConfiguration before singletons and main function
 * This is something like Q_DECLARE_METATYPE but for operators
 * We should create a Q_DECLARE_METATYPE_STREAM_OPERATORS defined function to deal with future cases
 * For any further explanation:
 * http://doc.qt.io/qt-5/qmetatype.html#Q_DECLARE_METATYPE
 * https://code.woboq.org/qt5/qtbase/src/corelib/kernel/qmetatype.h.html#_M/Q_DECLARE_METATYPE
 * https://code.woboq.org/qt5/qtbase/src/corelib/kernel/qmetatype.h.html#_Z32qRegisterMetaTypeStreamOperatorsPKcPT_
 */
struct LinkConfigurationRegisterStruct {
    LinkConfigurationRegisterStruct()
    {
        static QBasicAtomicInt metatypeId = Q_BASIC_ATOMIC_INITIALIZER(0);
        if (const int id = metatypeId.loadAcquire()) {
            return;
        }

        const int newId = qRegisterMetaType<LinkConfiguration>("LinkConfiguration");
        metatypeId.storeRelease(newId);
        qRegisterMetaTypeStreamOperators<LinkConfiguration>("LinkConfiguration");
    }
};

namespace
{
LinkConfigurationRegisterStruct _linkConfigurationRegisterStruct;
}
