add_library(
    network
STATIC
    networkmanager.cpp
    networktool.cpp
)

target_link_libraries(
    network
PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Gui
    Qt6::Widgets
    Qt6::Quick
)
