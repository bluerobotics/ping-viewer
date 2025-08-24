add_library(
    util
STATIC
    util.cpp
)

target_link_libraries(
    util
PRIVATE
    Qt6::Charts
    Qt6::Core
    Qt6::Qml
    Qt6::Widgets
    Qt6::SerialPort
)
