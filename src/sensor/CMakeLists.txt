add_library(
    sensor
STATIC
    hexvalidator.cpp
    ping.cpp
    ping360.cpp
    ping360helperservice.cpp
    ping360asciiprotocol.cpp
    pingparserext.cpp
    pingsensor.cpp
    protocoldetector.cpp
    sensor.cpp
    parser.h # for the moc.
)

target_link_libraries(
    sensor
PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Quick
    Qt6::SerialPort
    Qt6::Widgets # because of stylemanager
    Qt6::Concurrent

    mavlink
    network
)
