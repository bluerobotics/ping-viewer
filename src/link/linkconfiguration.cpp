#include <QUrl>

#include "linkconfiguration.h"

const QMap<LinkConfiguration::Error, QString> LinkConfiguration::_errorMap {
    {NoErrors, "No link configuration errors."},
    {MissingConfiguration, "Warning: Link configuration is missing."},
    {NoType, "Link configuration Type is None."},
    {InvalidType, "Link configuration Type is invalid."},
    {NoArgs, "Link configuration does not have arguments."},
    {InvalidArgsNumber, "Link configuration have a invalid number of arguments"},
    {ArgsAreEmpty, "Link configuration arguments are empty."},
    {InvalidUrl, "Url not formatted properly."},
};

const QString LinkConfiguration::createFullConfString() const
{
    QStringList args {_linkConf.args};
    args.prepend(QString::number(_linkConf.type));
    return args.join(":");
}

const QStringList LinkConfiguration::createFullConfStringList() const
{
    QStringList args {_linkConf.args};
    args.prepend(QString::number(_linkConf.type));
    return args;
}

LinkConfiguration::Error LinkConfiguration::error() const
{
    // No type, no valid connection
    if (_linkConf.type == LinkType::None) {
        return NoType;
    }

    // Type is not declared in configuration link type
    if (_linkConf.type < LinkType::None || _linkConf.type >= LinkType::Last) {
        return InvalidType;
    }

    // Name is not necessary to do a connection
    if (!_linkConf.name.isEmpty()) {
        return MissingConfiguration;
    }

    // Simulation does not need args
    // TODO: rework this check, check linkconfiguration capabilities or add new ones for this case
    if ((_linkConf.type == LinkType::Ping1DSimulation || _linkConf.type == LinkType::Ping360Simulation)
        && !_linkConf.args.length()) {
        return NoErrors;
    }

    // Everything else does need args
    if (!_linkConf.args.length()) {
        return NoArgs;
    }

    // Usually connections are made with path:format/conf
    if (_linkConf.args.length() != 2) {
        return InvalidArgsNumber;
    }

    // Args must be no empty strings
    for (const auto& arg : _linkConf.args) {
        if (arg.isEmpty()) {
            return InvalidArgsNumber;
        }
    }

    if (_linkConf.type == LinkType::Udp) {
        if (!QUrl(_linkConf.args[0]).isValid()) {
            return InvalidUrl;
        }
    }

    return NoErrors;
}

void LinkConfiguration::setArgs(const QStringList& args)
{
    _linkConf.args = args;
    emit configurationChanged();
}

QString LinkConfiguration::serialPort()
{
    if (!checkType(LinkType::Serial) || !_linkConf.args.size()) {
        return QString();
    }
    return _linkConf.args[0];
}

int LinkConfiguration::serialBaudrate()
{
    if (!checkType(LinkType::Serial) || _linkConf.args.size() < 1) {
        return 0;
    }

    return _linkConf.args[1].toInt();
}

QString LinkConfiguration::udpHost()
{
    if (!checkType(LinkType::Udp) || !_linkConf.args.size()) {
        return QString();
    }

    return _linkConf.args[0];
}

int LinkConfiguration::udpPort()
{
    if (!checkType(LinkType::Udp) || _linkConf.args.size() < 1) {
        return 0;
    }

    return _linkConf.args[1].toInt();
}

bool operator==(const LinkConfiguration& first, const LinkConfiguration& second)
{
    auto firstLinkconf = first.configurationStruct();
    auto secondLinkconf = second.configurationStruct();
    return (firstLinkconf.name == secondLinkconf.name) && (firstLinkconf.type == secondLinkconf.type)
        && (firstLinkconf.args == secondLinkconf.args) && (firstLinkconf.deviceType == secondLinkconf.deviceType);
}

QDebug operator<<(QDebug d, const LinkConfiguration& other)
{
    QString text(QStringLiteral("LinkConfiguration{Name: %1, Sensor: %2, LinkType: %3, Arguments: (%4)}"));
    d << text.arg(other.name(), PingHelper::nameFromDeviceType(other.deviceType()), other.typeToString(),
        other.args()->join(":"));
    return d;
}

QDataStream& operator<<(QDataStream& out, const LinkConfiguration linkConfiguration)
{
    out << linkConfiguration.configurationStructPtr()->name;
    out << QVariant(linkConfiguration.configurationStructPtr()->args);
    out << QVariant(linkConfiguration.configurationStructPtr()->type);
    // TODO: We need to register PingDeviceType
    out << QVariant(static_cast<int>(linkConfiguration.configurationStructPtr()->deviceType));
    return out;
}

QDataStream& operator>>(QDataStream& in, LinkConfiguration& linkConfiguration)
{
    QString name;
    QVariant variantArgs, variantType, variantDeviceType;
    in >> name;
    in >> variantArgs;
    in >> variantType;
    in >> variantDeviceType;
    linkConfiguration = LinkConfiguration(variantType.value<LinkType>(), variantArgs.toStringList(), name,
        static_cast<PingDeviceType>(variantDeviceType.toInt()));
    emit linkConfiguration.configurationChanged();
    return in;
}
