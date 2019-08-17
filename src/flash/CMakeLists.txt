add_library(
    flash
STATIC
    flasher.cpp
)

target_link_libraries(
    flash
PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::SerialPort
)
