option(AUTO_KILL "Turn on auto-kill mode" OFF)
if(AUTO_KILL)
    message(STATUS "Auto-kill mode enabled")
    add_compile_definitions(
        AUTO_KILL=ON
    )
endif()

option(PING360_SPEED_TEST "Turn on Ping360 speed test mode" OFF)
if(PING360_SPEED_TEST)
    message(STATUS "Ping360 speed test mode enabled")
    add_compile_definitions(
        PING360_SPEED_TEST=ON
    )
endif()