add_library(
    util
STATIC
    util.cpp
)

target_link_libraries(
    util
PRIVATE
    Qt5::Charts
    Qt5::Core
    Qt5::Qml
    Qt5::Widgets
    Qt5::SerialPort
)
