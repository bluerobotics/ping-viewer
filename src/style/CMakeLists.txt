add_library(
    style
STATIC
    stylemanager.cpp
)

target_link_libraries(
    style
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Qml
)
