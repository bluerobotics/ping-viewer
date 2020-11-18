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
qtquick_compiler_add_resources(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../resources.qrc)

# Configure icon for mac build
set(MACOSX_BUNDLE_ICON_FILE icon.icns)
set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/../deploy/${MACOSX_BUNDLE_ICON_FILE})
set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

set(SOURCES
    MACOSX_BUNDLE
    main.cpp
    ${APP_ICON}
    ${RESOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ping-cpp/ping-cpp/src/message/ping-message-all.h
)

add_executable(pingviewer
    ${SOURCES}
)

target_link_libraries(
    pingviewer
PRIVATE
    Qt5::Core
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::Charts
    Qt5::Svg
    Qt5::Widgets # QApplication
    ${INCLUDE_DIRS}
    fmt::fmt
)
