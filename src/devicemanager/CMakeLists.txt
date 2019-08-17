add_library(
    devicemanager
STATIC
    devicemanager.cpp
)

target_link_libraries(devicemanager
PRIVATE
    Qt5::Core
    Qt5::Network
    Qt5::Qml
    Qt5::Quick
)