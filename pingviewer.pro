TARGET = pingviewer

# windows
RC_ICONS = deploy/icon.ico
# mac
ICON = deploy/icon.icns

CONFIG += \
    c++17 \
    qtquickcompiler \
    static

QT += core charts gui qml quick widgets quickcontrols2 concurrent svg xml

include($$PWD/src/src.pri)

RESOURCES += \
    resources.qrc

*-g++ {
    QMAKE_CXXFLAGS += -fopenmp -fdiagnostics-color=always
    QMAKE_LFLAGS += -fopenmp
}

# Warning as error
*-g++ | *-clang {
    QMAKE_CXXFLAGS += -Werror -Ofast
}
*msvc {
    QMAKE_CXXFLAGS += /WX \
        /wd4305 \ # Remove truncated warnings, msvc does not provide some non double values

    # Necessary for register changes in serial layer for timer latency
    QMAKE_LFLAGS += /MANIFESTUAC:"level='requireAdministrator'"
}

include(lib/maddy/maddy.pri)
include(lib/ping-cpp/ping-cpp.pri)

CONFIG(debug, debug|release) {
    message("Debug Build !")
    win32 {
        # Windows debug
        CONFIG += console
        INCLUDEPATH += "$$PWD/drkonqi/KCrash"
        LIBS += "$$PWD/drkonqi/KF5Crash.lib"
    }
} else {
    message("Release Build !")
}

# Enable ccache where we can
linux|macx|ios {
    system(which ccache) {
        message("Found ccache, enabling in..")
        !ios {
            QMAKE_CXX = ccache $$QMAKE_CXX
            QMAKE_CC  = ccache $$QMAKE_CC
        } else {
            QMAKE_CXX = $$PWD/tools/iosccachecc.sh
            QMAKE_CC  = $$PWD/tools/iosccachecxx.sh
        }
    }
}

message("The project contains the following files:")
message("Headers:      " $$HEADERS)
message("Sources:      " $$SOURCES)
message("Resources:    " $$RESOURCES)
message("QT:           " $$QT)
message("Config:       " $$CONFIG)
message("CXX flags:    " $$QMAKE_CXXFLAGS)
message("L flags:      " $$QMAKE_LFLAGS)
message("Libs:         " $$LIBS)
message("Compiler:     " $$QMAKE_COMPILER_DEFINES)

# https://git-scm.com/docs/git-log placeholders
# Get git info
message("---")
message("Check for GIT version and tag:")
exists ($$_PRO_FILE_PWD_/.git) {
    GIT_VERSION = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$PWD log -1 --format=%h)
    GIT_VERSION_DATE = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$PWD log -1 --format=%aI)
    GIT_TAG = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$PWD tag --points-at HEAD | grep -v continuous | grep -v stable)
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
