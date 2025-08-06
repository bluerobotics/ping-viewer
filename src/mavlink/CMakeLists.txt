add_library(
    mavlink
STATIC
    mavlinkmanager.cpp
)

target_link_libraries(
    mavlink
PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Quick
)
