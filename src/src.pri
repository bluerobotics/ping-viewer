INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/*.h

SOURCES += \
    $$PWD/*.cpp

include($$PWD/sensor/sensor.pri)
include($$PWD/settings/settings.pri)
