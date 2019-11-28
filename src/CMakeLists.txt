set(INCLUDE_DIRS
    commandline
    devicemanager
    filemanager
    flash
    link
    logger
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

add_executable(ping-viewer
    main.cpp
    ../resources.qrc
    ../lib/ping-cpp/ping-cpp/src/message/ping-message-all.h
)

target_link_libraries(
    ping-viewer
    Qt5::Core
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Charts
    Qt5::Widgets # QApplication

    ${INCLUDE_DIRS}
)
