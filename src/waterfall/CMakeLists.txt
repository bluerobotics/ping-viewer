add_library(
    waterfall
STATIC
    gradientscale.cpp
    polarplot.cpp
    waterfall.cpp
    waterfallgradient.cpp
    waterfallplot.cpp
)

target_link_libraries(
    waterfall
PRIVATE
    Qt6::Concurrent
    Qt6::Quick
    logger
)
