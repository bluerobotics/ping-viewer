TARGET = pingviewer

RC_ICONS = deploy/icon.ico

CONFIG += \
    c++14 \
    static

QT += core charts gui qml quick widgets quickcontrols2 concurrent svg xml

HEADERS += \
    src/waterfallgradient.h \
    src/waterfall.h \
    src/logger.h \
    src/loglistmodel.h \
    src/util.h

SOURCES += \
    src/waterfallgradient.cpp \
    src/waterfall.cpp \
    src/logger.cpp \
    src/loglistmodel.cpp \
    src/util.cpp \
    src/main.cpp

include($$PWD/src/sensor/sensor.pri)

RESOURCES += \
    resources.qrc

*-g++ {
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
}

include(lib/ping-protocol-cpp/ping.pri)

build_pass:CONFIG(debug, debug|release) {
    message("Debug Build !")
    win32 {
        # Windows debug
        CONFIG += console
    }
} else {
    message("Release Build !")
}

message("The project contains the following files:")
message("Headers:      " $$HEADERS)
message("Sources:      " $$SOURCES)
message("Resources:    " $$RESOURCES)
message("QT:           " $$QT)
message("Config:       " $$CONFIG)
message("CXX flags:    " $$QMAKE_CXXFLAGS)
message("L flags:      " $$QMAKE_LFLAGS)

# https://git-scm.com/docs/git-log placeholders
# Get git info
message("---")
message("Check for GIT version and tag:")
exists ($$_PRO_FILE_PWD_/.git) {
    GIT_VERSION = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$PWD log -1 --format=%h)
    GIT_VERSION_DATE = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$PWD log -1 --format=%aI)
    GIT_TAG = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$PWD describe --abbrev=0 --tags)
    GIT_URL = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$PWD remote get-url origin)
    DEFINES += 'GIT_VERSION=\\"$$GIT_VERSION\\"'
    DEFINES += 'GIT_VERSION_DATE=\\"$$GIT_VERSION_DATE\\"'
    DEFINES += 'GIT_TAG=\\"$$GIT_TAG\\"'
    DEFINES += 'GIT_URL=\\"$$GIT_URL\\"'
} else {
    DEFINES += 'GIT_VERSION=\\"-\\"'
    DEFINES += 'GIT_VERSION_DATE=\\"-\\"'
    DEFINES += 'GIT_TAG=\\"-\\"'
    DEFINES += 'GIT_URL=\\"-\\"'
}

message(" GIT_VERSION:      " $$GIT_VERSION)
message(" GIT_VERSION_DATE: " $$GIT_VERSION_DATE)
message(" GIT_TAG:          " $$GIT_TAG)
message(" GIT_URL:          " $$GIT_URL)