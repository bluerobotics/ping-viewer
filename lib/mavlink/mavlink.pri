# We only include the necessary headers to provide a minimum functionality
# MavlinkManager is the one that will describe which messages are included

INCLUDEPATH += \
    $$PWD/c_library_v2/ \
    $$PWD/c_library_v2/minimal/ \
    $$PWD/c_library_v2/common/ \

HEADERS += \
    $$PWD/c_library_v2/*.h \
    $$PWD/c_library_v2/minimal/*.h \
