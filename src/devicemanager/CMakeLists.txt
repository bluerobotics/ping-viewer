add_library(
    devicemanager
STATIC
    devicemanager.cpp
)

target_link_libraries(
    devicemanager
PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Qml
    Qt6::Quick
)
