include_directories(.)

add_subdirectory(devicemanager)
add_subdirectory(filemanager)
add_subdirectory(flash)
add_subdirectory(link)
add_subdirectory(logger)
add_subdirectory(network)
add_subdirectory(notification)
add_subdirectory(qtquickcomponents)
add_subdirectory(sensor)
add_subdirectory(settings)
add_subdirectory(style)
add_subdirectory(util)
add_subdirectory(waterfall)

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
    devicemanager
    filemanager
    flash
    link
    logger
    network
    notification
    qtquickcomponents
    sensor
    settings
    style
    util
    waterfall
)