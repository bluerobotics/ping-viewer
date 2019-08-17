add_library(
    style
STATIC
    stylemanager.cpp
)

target_link_libraries(style
PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Qml
)