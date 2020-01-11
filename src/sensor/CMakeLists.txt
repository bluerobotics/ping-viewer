add_library(
    sensor
STATIC
    hexvalidator.cpp
    ping.cpp
    ping360.cpp
    ping360ethernetfinder.cpp
    pingparserext.cpp
    pingsensor.cpp
    protocoldetector.cpp
    sensor.cpp
    parser.h # for the moc.
)

target_link_libraries(
    sensor
PRIVATE
    Qt5::Core
    Qt5::Network
    Qt5::Quick
    Qt5::SerialPort
    Qt5::Widgets # because of stylemanager
    Qt5::Concurrent

    mavlink
    network
)
