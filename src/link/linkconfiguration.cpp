#include "linkconfiguration.h"

const QMap<LinkConfiguration::Error, QString> LinkConfiguration::errorMap {
    {NoErrors, "No link configuration errors."},
    {MissingConfiguration, "Warning: Link configuration is missing."},
    {NoType, "Link configuration Type is None."},
    {InvalidType, "Link configuration Type is invalid."},
    {NoArgs, "Link configuration does not have arguments."},
    {InvalidArgsNumber, "Link configuration have a invalid number of arguments"},
    {ArgsAreEmpty, "Link configuration arguments are empty."},
};

const QString LinkConfiguration::createFullConfString() const
{
    QStringList args{_linkConf.args};
    args.prepend(QString::number(_linkConf.type));
    return args.join(":");
}

const QStringList LinkConfiguration::createFullConfStringList() const
{
    QStringList args{_linkConf.args};
    args.prepend(QString::number(_linkConf.type));
    return args;
}

LinkConfiguration::Error LinkConfiguration::error() const
{
    // No type, no valid connection
    if(_linkConf.type == LinkType::None) {
        return NoType;
    }

    // Type is not declared in configuration link type
    if(_linkConf.type < LinkType::None || _linkConf.type >= LinkType::Last) {
        return InvalidType;
    }

    // Name is not necessary to do a connection
    if(!_linkConf.name.isEmpty()) {
        return MissingConfiguration;
    }

    // Simulation does not need args
    if(_linkConf.type == LinkType::PingSimulation && !_linkConf.args.length()) {
        return NoErrors;
    }

    // Everything else does need args
    if(!_linkConf.args.length()) {
        return NoArgs;
    }

    // Usually connections are made with path:format/conf
    if(_linkConf.args.length() != 2) {
        return InvalidArgsNumber;
    }

    // Args must be no empty strings
    for( const auto& arg : _linkConf.args) {
        if(arg.isEmpty()) {
            return InvalidArgsNumber;
        }
    }

    return NoErrors;
}

//Todo: move to `operator QString()`
QString LinkConfiguration::toString() const
{
    QString text(QStringLiteral("LinkConfiguration{Name: %1, LinkType: %2, Arguments: (%3)}"));
    return text.arg(name(), QString::number(type()), args()->join(":"));
}