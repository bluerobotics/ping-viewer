add_library(
    waterfall
STATIC
    polarplot.cpp
    waterfall.cpp
    waterfallgradient.cpp
    waterfallplot.cpp
)

target_link_libraries(
    waterfall
    PRIVATE
    Qt5::Concurrent
    Qt5::Quick
    logger
    )
