#pragma once

#include <QApplication>
#include <QColor>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>

class QJSEngine;
class QQmlEngine;

Q_DECLARE_LOGGING_CATEGORY(STYLEMANAGER)

// TODO: Move this macro to somewhere else and use it in other classes
#define P_PROPERTY(TYPE, NAME, VALUE) \
    Q_PROPERTY(TYPE NAME READ NAME WRITE NAME NOTIFY NAME##Changed) \
public: \
    TYPE NAME() { return _##NAME; }; \
    void NAME(TYPE NAME) { if(NAME == _##NAME) return; _##NAME = NAME; emit NAME##Changed(); }; \
    Q_SIGNAL void NAME##Changed();\
private: \
    TYPE _##NAME = VALUE;

/**
 * @brief Manage the project StyleManager
 *
 */
class StyleManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Return StyleManager pointer
     *
     * @return StyleManager*
     */
    static StyleManager* self();

    /**
     * @brief Set the Qml Engine object
     *
     * @param engine
     */
    void setQmlEngine(QQmlApplicationEngine* engine) { _engine = engine; }

    /**
     * @brief Get display size
     *  For QML, it's possible to use main root object "window"
     * @return QSize
     */
    QSize displaySize() const;

    /**
     * @brief Return a pointer of this singleton to the qml register function
     *
     * @param engine
     * @param scriptEngine
     * @return QObject*
     */
    static QObject* qmlSingletonRegister(QQmlEngine* engine, QJSEngine* scriptEngine);

#define CREATE_ICON(NAME) Q_INVOKABLE static QString NAME##Icon() { return iconNameHelper(#NAME); };
    // This list can be generated with: `for f in *; do echo "CREATE_ICON(${f%".svg"})"; done;`
    CREATE_ICON(arrow)
    CREATE_ICON(bell)
    CREATE_ICON(chat)
    CREATE_ICON(chip)
    CREATE_ICON(clear)
    CREATE_ICON(close)
    CREATE_ICON(configure)
    CREATE_ICON(connect)
    CREATE_ICON(disk)
    CREATE_ICON(info)
    CREATE_ICON(lock)
    CREATE_ICON(report)
    CREATE_ICON(reset)
    CREATE_ICON(settings)
    CREATE_ICON(sun)
    CREATE_ICON(unlock)

    P_PROPERTY(bool, isDark, false)
    P_PROPERTY(QColor, primaryColor, {})
    P_PROPERTY(QColor, secondaryColor, {})

public:
    /**
     * @brief Themes available
     *  QQuickMaterialStyle is private, we can't access the Theme enum directly
     *
     */
    enum Theme {
        Light,
        Dark,
        System
    };
    Q_ENUM(Theme)

    P_PROPERTY(int, theme, Theme::Dark)

private:
    Q_DISABLE_COPY(StyleManager)
    /**
     * @brief Construct a new Style Manager object
     *
     */
    StyleManager();

    static QString iconNameHelper(QString functionName)
    {
        return QStringLiteral("/icons/%1.svg").arg(functionName.remove("Icon"));
    };

    QColor _dark = "black";
    QColor _light = "linen";
    QQmlApplicationEngine* _engine;
};
