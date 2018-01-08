TARGET = pingviewer

CONFIG += \
    c++14 \
    static

QT += core gui qml quick widgets quickcontrols2

HEADERS += \
    src/waterfallgradient.h \
    src/waterfall.h \
    src/logger.h

SOURCES += \
    src/waterfallgradient.cpp \
    src/waterfall.cpp \
    src/logger.cpp \
    src/main.cpp

RESOURCES += \
    resources.qrc

# https://git-scm.com/docs/git-log placeholders
# Get git info
GIT_VERSION = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$PWD log -1 --format=%h)
GIT_VERSION_DATE = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$PWD log -1 --format=%aI)
GIT_TAG = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$PWD describe --abbrev=0)
GIT_URL = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$PWD remote get-url origin)
DEFINES += 'GIT_VERSION=\\"$$GIT_VERSION\\"'
DEFINES += 'GIT_VERSION_DATE=\\"$$GIT_VERSION_DATE\\"'
DEFINES += 'GIT_TAG=\\"$$GIT_TAG\\"'
DEFINES += 'GIT_URL=\\"$$GIT_URL\\"'