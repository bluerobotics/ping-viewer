INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/*.h

SOURCES += \
    $$PWD/*.cpp

include($$PWD/link/link.pri)
include($$PWD/sensor/sensor.pri)
include($$PWD/settings/settings.pri)
