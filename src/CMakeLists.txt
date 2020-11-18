set(INCLUDE_DIRS
    commandline
    devicemanager
    filemanager
    flash
    link
    logger
    mavlink
    network
    notification
    sensor
    settings
    style
    util
    waterfall
)

include_directories(. ${INCLUDE_DIRS})

foreach(directory ${INCLUDE_DIRS})
    add_subdirectory(${directory})
endforeach()

find_package(Qt5QuickCompiler)
qtquick_compiler_add_resources(RESOURCES ../resources.qrc)

add_executable(pingviewer
    MACOSX_BUNDLE
    main.cpp
    ${RESOURCES}
    ../lib/ping-cpp/ping-cpp/src/message/ping-message-all.h
)

target_link_libraries(
    pingviewer
PRIVATE
    Qt5::Core
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Charts
    Qt5::Widgets # QApplication
    ${INCLUDE_DIRS}
    fmt::fmt
)
