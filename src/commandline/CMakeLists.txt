add_library(
    commandline
STATIC
    commandlineparser.cpp
)

target_link_libraries(
    commandline
PRIVATE
    Qt6::Core
    Qt6::Network
    Qt6::Quick
)
