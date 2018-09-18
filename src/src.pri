INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/filemanager.h \
    $$PWD/log*.h \
    $$PWD/util.h \
    $$PWD/waterfall*.h \

SOURCES += \
    $$PWD/filemanager.cpp \
    $$PWD/log*.cpp \
    $$PWD/util.cpp \
    $$PWD/waterfall*.cpp \

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

include($$PWD/link/link.pri)
include($$PWD/sensor/sensor.pri)
include($$PWD/settings/settings.pri)
