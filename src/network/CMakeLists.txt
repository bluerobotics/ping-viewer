add_library(
    network
STATIC
    networkmanager.cpp
    networktool.cpp
)

target_link_libraries(
    network
PRIVATE
    Qt5::Core
    Qt5::Network
    Qt5::Gui
    Qt5::Widgets
    Qt5::Quick
)
