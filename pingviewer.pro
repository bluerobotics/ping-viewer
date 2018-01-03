TARGET = pingviewer

CONFIG += \
    c++14 \
    static

QT += core gui qml quick widgets

SOURCES += \
    src/main.cpp

RESOURCES += \
    resources.qrc

# Get git version
GIT_VERSION = $$system(git --git-dir $$_PRO_FILE_PWD_/.git --work-tree $$PWD describe --always --tags)
DEFINES += 'GIT_VERSION=\\"$$GIT_VERSION\\"'