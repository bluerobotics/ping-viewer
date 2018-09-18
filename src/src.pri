INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/util.h \

SOURCES += \
    $$PWD/util.cpp \

test {
    message(Configuring test build...)

    QT += testlib

    HEADERS += \
        $$PWD/test.h

    SOURCES += \
        $$PWD/test.cpp
} else {
    SOURCES += \
        $$PWD/main.cpp
}

include($$PWD/filemanager/filemanager.pri)
include($$PWD/link/link.pri)
include($$PWD/logger/logger.pri)
include($$PWD/sensor/sensor.pri)
include($$PWD/settings/settings.pri)
include($$PWD/waterfall/waterfall.pri)