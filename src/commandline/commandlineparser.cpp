#include "commandlineparser.h"
#include "linkconfiguration.h"
#include "logger.h"

PING_LOGGING_CATEGORY(COMMANDLINEPARSER, "ping.commandlineparser");

CommandLineParser::CommandLineParser(const QCoreApplication& app)
    : QCommandLineParser()
{
    setApplicationDescription("Graphical user interface for the Blue Robotics Ping1D and Ping360.");
    addHelpOption();
    addVersionOption();

    for (const auto& optionStruct : _optionsStruct) {
        addOption(optionStruct.option);
    }

    // Parser the main application
    process(app);

    for (const auto& optionStruct : _optionsStruct) {
        QString result = value(optionStruct.option);
        if (!result.isEmpty()) {
            qCDebug(COMMANDLINEPARSER) << QStringLiteral("Valid option: %1 [%2]: %3")
                                              .arg(optionStruct.option.names().first(),
                                                  optionStruct.option.description(), result);
            optionStruct.function(result);
        }
    }
};
