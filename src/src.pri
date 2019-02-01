INCLUDEPATH += $$PWD

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
include($$PWD/flash/flash.pri)
include($$PWD/link/link.pri)
include($$PWD/logger/logger.pri)
include($$PWD/network/network.pri)
include($$PWD/notification/notification.pri)
include($$PWD/sensor/sensor.pri)
include($$PWD/settings/settings.pri)
include($$PWD/style/style.pri)
include($$PWD/util/util.pri)
include($$PWD/waterfall/waterfall.pri)
