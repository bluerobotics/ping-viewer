#pragma once

#include <QLoggingCategory>

class QJSEngine;
class QQmlEngine;

Q_DECLARE_LOGGING_CATEGORY(NETWORKTOOL)

/**
 * @brief Manage the project NetworkTool
 *
 */
class NetworkTool : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Return NetworkTool pointer
     *
     * @return NetworkTool*
     */
    static NetworkTool* self();


    /**
     * @brief Check if a new interface version is available
     *
     */
    static void checkInterfaceUpdate();

    /**
     * @brief Parser the json payload to check for new versions
     *
     * @param jsonDocument
     */
    static void checkNewVersionInGitHubPayload(QJsonDocument& jsonDocument);

    /**
     * @brief Schedule a update check
     *
     */
    void scheduleUpdateCheck();

    /**
     * @brief Return a pointer of this singleton to the qml register function
     *
     * @param engine
     * @param scriptEngine
     * @return QObject*
     */
    static QObject* qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine);
private:
    Q_DISABLE_COPY(NetworkTool)
    /**
     * @brief Construct a new Network Tool object
     *
     */
    NetworkTool() = default;
};
