add_library(
    flash
STATIC
    flasher.cpp
)

target_link_libraries(
    flash
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::SerialPort
)
