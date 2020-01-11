add_library(
    commandline
STATIC
    commandlineparser.cpp
)

target_link_libraries(
    commandline
PRIVATE
    Qt5::Core
    Qt5::Network
    Qt5::Quick
)
