/**
 * @brief This header defines some macros to help SettingsManager development
 */

#include <QVariant>

/**
 *@brief call the correct method for Qvariants that have bool values
 */
template<bool> bool qVariantValueOf(const QVariant& variant)
{
    return variant.toBool();
}

/**
 *@brief call the correct method for Qvariants that have int values
 */
template<int> int qVariantValueOf(const QVariant& variant)
{
    return variant.toInt();
}

/**
 *@brief call the correct method for Qvariants that have T values
 */
template<typename T> T qVariantValueOf(const QVariant& variant)
{
    return variant.value<T>();
}

/**
 * @brief Defines a simple propriety
 * The output will be something like:
 * AUTO_PROPERTY(myType, myName, defaultValue):
 *    Q_PROPERTY(myType myName READ myName WRITE myName NOTIFY myNameChanged)
 *public:
 *    // Get property
 *    myType myName() {
 *        // Always check settings content, make sure that everything is updated
 *        QVariant variant = _settings.value(QStringLiteral(myName)); \
 *        if(variant.isValid()) { \
 *            _myName = ::qVariantValueOf<myType>(variant); \
 *        } else { \
 *            _myName = defaultValue; \
 *            myType(_myType); \
 *        } \
 *        qCDebug(SETTINGSMANAGER) << QStringLiteral("Load %1 with:").arg("myName") << _myName;\
 *        return _myType; \
 *    }
 *    // Change property value
 *    void myName(myType value) {
 *        if(_myName == value) { return; }
 *        _myName = value;
 *        _settings.setValue(QStringLiteral("myName"), qVariantFromValue(value));
 *        _settings.sync();
 *        qCDebug(SETTINGSMANAGER) << QStringLiteral("Save %1 with:").arg("myName") << value;
 *        emit myNameChanged();
 *    }
 *    Q_SIGNAL void myNameChanged();
 *private:
 *    // Define internal variable
 *    myType _myName;
 */
#define AUTO_PROPERTY(TYPE, NAME, DEFAULT_VALUE) \
    Q_PROPERTY(TYPE NAME READ NAME WRITE NAME NOTIFY NAME ## Changed ) \
public: \
    TYPE NAME() { \
        QVariant variant = _settings.value(QStringLiteral(#NAME)); \
        if(variant.isValid()) { \
            _ ## NAME = ::qVariantValueOf<TYPE>(variant); \
        } else { \
            _ ## NAME = DEFAULT_VALUE; \
            NAME(_ ## NAME); \
        } \
        qCDebug(SETTINGSMANAGER) << QStringLiteral("Load %1 with:").arg(#NAME) << _ ## NAME;\
        return _ ## NAME; \
    } \
    void NAME(TYPE value) { \
        if(_ ## NAME == value) { return; }\
        _ ## NAME = value; \
        _settings.setValue(QStringLiteral(#NAME), qVariantFromValue(value)); \
        _settings.sync(); \
        qCDebug(SETTINGSMANAGER) << QStringLiteral("Save %1 with:").arg(#NAME) << value;\
        emit NAME ## Changed(); \
    } \
    Q_SIGNAL void NAME ## Changed();\
private: \
    TYPE _ ## NAME;

/**
 * @brief Create a model property for qml connections
 * The output will be something like:
 * AUTO_PROPERTY_MODEL(myType, myName, myModelType, {model_var_1, model_var_2}):
 *    Q_PROPERTY(myType myName READ myName WRITE myName NOTIFY myNameChanged)
 *    Q_PROPERTY(const myModelType* myNameModel READ myNameModel)
 *public:
 *    // Get property
 *    myType myName() {
 *        // Always check settings content, make sure that everything is updated
 *        _myName = _settings.value(QStringLiteral("myName")).value<myType>();
 *        return _myName;
 *    }
 *    // Change property value
 *    void myName(myType value) {
 *        // If variable has the same value, the same thing exist inside settings
 *        if(_myName == value) { return; }
 *        _myName = value;
 *        _settings.setValue(QStringLiteral("myName"), value);
 *        qCDebug(SETTINGSMANAGER) << QStringLiteral("Save %1 with:").arg("myName") << value;
 *        emit myNameChanged();
 *    }
 *    // Get myName model
 *    const myModelType* myNameModel() { return _myNameModel; }
 *    Q_SIGNAL void myNameChanged();
 *private:
 *    // Define internal variable and its model
 *    myType _myName;
 *    const myModelType* _myNameModel{new myModelType({model_var_1, model_var_2})};
 */
#define AUTO_PROPERTY_MODEL(TYPE, NAME, MODEL_TYPE, MODEL_LIST) \
    Q_PROPERTY(TYPE NAME READ NAME WRITE NAME NOTIFY NAME ## Changed ) \
    Q_PROPERTY(const MODEL_TYPE* NAME ## Model READ NAME ## Model CONSTANT ) \
public: \
    TYPE NAME() { _ ## NAME = _settings.value(QStringLiteral(#NAME)).value<TYPE>(); return _ ## NAME ; } \
    void NAME(TYPE value) { \
        if(_ ## NAME == value) { return; }\
        _ ## NAME = value; \
        _settings.setValue(QStringLiteral(#NAME), value); \
        qCDebug(SETTINGSMANAGER) << QStringLiteral("Save %1 with:").arg(#NAME) << value;\
        emit NAME ## Changed(); \
    } \
    const MODEL_TYPE* NAME ## Model() { return _ ## NAME ## Model; }\
    Q_SIGNAL void NAME ## Changed();\
private: \
    TYPE _ ## NAME; \
    const MODEL_TYPE* _ ## NAME ## Model{new MODEL_TYPE(MODEL_LIST)};

/**
 * @brief Create a model property for qml connections with QJsonSettings
 * The output will be something like:
 * #define AUTO_PROPERTY_JSONMODEL(myName, myJson) \
 *    Q_PROPERTY(int myNameIndex READ myNameIndex WRITE myNameIndex NOTIFY myNameIndexChanged ) \
 *    Q_PROPERTY(const QJsonSettings* myNameModel READ myNameModel ) \
 *    Q_PROPERTY(QJsonObject myName READ myName NOTIFY myNameIndexChanged ) \
 *public: \
 *    int myNameIndex() { _myNameIndex = _settings.value(QStringLiteral(myName)).value<int>(); return _myNameIndex ; } \
 *    void myNameIndex(int value) { \
 *        if(_myNameIndex == value) { return; }\
 *        _myNameIndex = value; \
 *        _settings.setValue(QStringLiteral(myName), value); \
 *        qCDebug(SETTINGSMANAGER) << QStringLiteral("Save %1 with:").arg(myName) << value;\
 *        emit myNameIndexChanged(); \
 *    } \
 *    const QJsonSettings* myNameModel() { return _myNameModel; }\
 *    QJsonObject myName() { return _myNameModel->object(myNameIndex()); } \
 *    Q_SIGNAL void myNameIndexChanged();\
private: \
    int _myNameIndex; \
    const QJsonSettings* _myNameModel{new QJsonSettings(myJson)};
 */
#define AUTO_PROPERTY_JSONMODEL(NAME, JSON) \
    Q_PROPERTY(int NAME ## Index READ NAME ## Index WRITE NAME ## Index NOTIFY NAME ## Index ## Changed ) \
    Q_PROPERTY(const QJsonSettings* NAME ## Model READ NAME ## Model CONSTANT) \
    Q_PROPERTY(QJsonObject NAME READ NAME NOTIFY NAME ## Index ## Changed ) \
public: \
    int NAME ## Index() { _ ## NAME ## Index = _settings.value(QStringLiteral(#NAME)).value<int>(); return _ ## NAME ## Index ; } \
    void NAME ## Index(int value) { \
        if(_ ## NAME ## Index == value) { return; }\
        _ ## NAME ## Index = value; \
        _settings.setValue(QStringLiteral(#NAME), value); \
        qCDebug(SETTINGSMANAGER) << QStringLiteral("Save %1 with:").arg(#NAME) << value;\
        emit NAME ## Index ## Changed(); \
    } \
    const QJsonSettings* NAME ## Model() { return _ ## NAME ## Model; }\
    QJsonObject NAME() { return _ ## NAME ## Model->object(NAME ## Index()); } \
    Q_SIGNAL void NAME ## Index ## Changed();\
private: \
    int _ ## NAME ## Index; \
    const QJsonSettings* _ ## NAME ## Model{new QJsonSettings(JSON)};

// Wrap model list
#define MODEL(...) \
    __VA_ARGS__
