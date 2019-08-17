add_library(
    qtquickcomponents
STATIC
    sliderruler.cpp
)

target_link_libraries(qtquickcomponents
PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::Quick
)