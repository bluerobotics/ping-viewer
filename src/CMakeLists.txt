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

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS
    QuickCompiler
)

qtquick_compiler_add_resources(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/../resources.qrc)

# Configure icon for mac build
set(MACOSX_BUNDLE_ICON_FILE icon.icns)
set(APP_ICON ${CMAKE_CURRENT_SOURCE_DIR}/../deploy/${MACOSX_BUNDLE_ICON_FILE})
set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

# Configure icon for windows build
set(WINDOWS_ICO_FILE icon.ico)
set(WINDOWS_RC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/../deploy/pingviewer.rc)
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/../deploy/pingviewer.rc.in
	${WINDOWS_RC_FILE}
)

set(SOURCES
    MACOSX_BUNDLE
    main.cpp
    ${APP_ICON}
    ${WINDOWS_RC_FILE}
    ${RESOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/ping-cpp/ping-cpp/src/message/ping-message-all.h
)

add_executable(pingviewer
    ${SOURCES}
)

# Set windows administrator request
if(WIN32 AND ENABLE_REQUIRE_ADMIN)
    set_target_properties(
        pingviewer
    PROPERTIES
        LINK_FLAGS "/level='requireAdministrator'"
    )
endif()

# Disable console when running in release mode,
# this is accomplished using the WIN32_EXCUTABLE property
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET pingviewer PROPERTY WIN32_EXECUTABLE true)
endif()

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

# Test target
if(NOT WIN32)
    enable_testing(true)
    add_executable(test test.cpp)
    add_test(NAME test COMMAND test)

    target_link_libraries(
        test
    PRIVATE
        Qt5::Core
        Qt5::Qml
        Qt5::Quick
        Qt5::QuickControls2
        Qt5::Charts
        Qt5::Svg
        Qt5::Test
        Qt5::Widgets
        ${INCLUDE_DIRS}
        fmt::fmt
    )
endif()
