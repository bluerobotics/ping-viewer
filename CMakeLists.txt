project(ping-viewer)
cmake_minimum_required(VERSION 3.8)

find_package(ECM 5.54.0 REQUIRED NO_MODULE)
set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

set(QT_MIN_VERSION "5.12.0")

set(CMAKE_AUTOMOC ON)
set(AUTOMOC_MOC_OPTIONS -Muri=org.bluerobotics.pingviewer)

set(CMAKE_AUTORCC ON)

add_definitions(
  -DQT_USE_QSTRINGBUILDER
  -DQT_NO_CAST_TO_ASCII
  -DQT_STRICT_ITERATORS
  -DQT_NO_CAST_FROM_BYTEARRAY
  -DQT_USE_FAST_OPERATOR_PLUS
)

# Set the Git variables here.
if(TRUE)
  set(GIT_VERSION "fake_version_fixme_later")
  set(GIT_VERSION_DATE "fake_version_date_fixme_later")
  set(GIT_TAG "fake_git_tag")
  set(GIT_URL "fake_git_url")

  add_definitions(-DGIT_VERSION="${GIT_VERSION}")
  add_definitions(-DGIT_VERSION_DATE="${GIT_VERSION_DATE}")
  add_definitions(-DGIT_TAG="${GIT_TAG}")
  add_definitions(-DGIT_URL="${GIT_URL}")
endif()

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS
    Charts
    Concurrent
    Core
    Gamepad
    Network
    Positioning
    Qml
    Quick
    QuickControls2
    SerialPort
    Widgets
)

# global include directories
include_directories(
    lib/ping-cpp/ping-cpp/src/message/
    lib/maddy/maddy/include/
)

add_subdirectory(src)
