#pragma once

#include "devicemanager.h"
#include <QCommandLineParser>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(COMMANDLINEPARSER)

/**
 * @brief Deal with command lines
 *
 */
class CommandLineParser : public QCommandLineParser {
public:
    /**
     * @brief Construct a new Command Line Parser object
     *  The constructor will deal with the arguments
     *
     * @param app
     */
    CommandLineParser(const QCoreApplication& app);

    /**
     * @brief Destroy the Command Line Parser object
     *
     */
    ~CommandLineParser() = default;

private:
    struct OptionStruct {
        QCommandLineOption option;
        std::function<void(const QString&)> function;
    };

    QList<OptionStruct> _optionsStruct {
        {
            {{"connect", "c"}, "Connect directly with the input.", "connectionString"
#if defined(PING360_SPEED_TEST)
                ,
                "Ping360:6"
#endif
            },
            [](const QString& result) { DeviceManager::self()->connectLinkDirectly(LinkConfiguration {result}); },
        },
    };
};
