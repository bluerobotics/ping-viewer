/**
 * @brief This header defines some macros to help SettingsManager development
 */

/**
 * @brief Defines a simple propriety
 * The output will be something like:
 * AUTO_PROPERTY(myType, myName):
 *    Q_PROPERTY(myType myName READ myName WRITE myName NOTIFY myNameChanged)
 *public:
 *    // Get property
 *    myType myName() {
 *        // Always check settings content, make sure that everything is updated
 *        _myName = _settings.value(QStringLiteral("myName")).value<myType>();
 *        return _myName;
 *    }
 *    // Change property value
 *    void myName(myType value) {
 *        if(_myName == value) { return; }
 *        _myName = value;
 *        _settings.setValue(QStringLiteral("myName"), value);
 *        qDebug(SETTINGSMANAGER) << QStringLiteral("Save %1 with:").arg("myName") << value;
 *        emit myNameChanged();
 *    }
 *    Q_SIGNAL void myNameChanged();
 *private:
 *    // Define internal variable
 *    myType _myName;
 */
#define AUTO_PROPERTY(TYPE, NAME) \
   Q_PROPERTY(TYPE NAME READ NAME WRITE NAME NOTIFY NAME ## Changed ) \
public: \
TYPE NAME() { _ ## NAME = _settings.value(QStringLiteral(#NAME)).value<TYPE>(); return _ ## NAME ; } \
   void NAME(TYPE value) { \
       if(_ ## NAME == value) { return; }\
       _ ## NAME = value; \
       _settings.setValue(QStringLiteral(#NAME), value); \
       qDebug(SETTINGSMANAGER) << QStringLiteral("Save %1 with:").arg(#NAME) << value;\
       emit NAME ## Changed(); \
   } \
   Q_SIGNAL void NAME ## Changed();\
private: \
   TYPE _ ## NAME;

/**
 * @brief Create a model property for qml connections
 * The output will be something like:
 * AUTO_PROPERTY_MODEL(TYPE, NAME, MODEL_TYPE, {model_var_1, model_var_2}):
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
 *        qDebug(SETTINGSMANAGER) << QStringLiteral("Save %1 with:").arg("myName") << value;
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
    Q_PROPERTY(const MODEL_TYPE* NAME ## Model READ NAME ## Model ) \
public: \
    TYPE NAME() { _ ## NAME = _settings.value(QStringLiteral(#NAME)).value<TYPE>(); return _ ## NAME ; } \
    void NAME(TYPE value) { \
        if(_ ## NAME == value) { return; }\
        _ ## NAME = value; \
        _settings.setValue(QStringLiteral(#NAME), value); \
        qDebug(SETTINGSMANAGER) << QStringLiteral("Save %1 with:").arg(#NAME) << value;\
        emit NAME ## Changed(); \
    } \
    const MODEL_TYPE* NAME ## Model() { return _ ## NAME ## Model; }\
    Q_SIGNAL void NAME ## Changed();\
private: \
    TYPE _ ## NAME; \
    const MODEL_TYPE* _ ## NAME ## Model{new MODEL_TYPE(MODEL_LIST)};

// Wrap model list
#define MODEL(...) \
    __VA_ARGS__
