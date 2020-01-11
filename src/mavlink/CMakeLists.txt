add_library(
    mavlink
STATIC
    mavlinkmanager.cpp
)

target_link_libraries(
    mavlink
PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Quick
)
