find_package(Git)

if(NOT GIT_FOUND OR NOT EXISTS "${PROJECT_SOURCE_DIR}/.git")
    return()
endif()

# Update submodules as needed
option(GIT_SUBMODULE "Check submodules during build" ON)
if(GIT_SUBMODULE)
    message(STATUS "Submodule update")
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
                    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
                    RESULT_VARIABLE GIT_SUBMODULE_RESULT)
    if(NOT GIT_SUBMODULE_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMODULE_RESULT}, please checkout submodules")
    endif()
endif()

# Fetch the necessary git variables
execute_process(COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${GIT_EXECUTABLE} log -1 --format=%aI
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_VERSION_DATE
                OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${GIT_EXECUTABLE} describe --match "[v,t]*" --tags --abbrev=0
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_TAG
                OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND ${GIT_EXECUTABLE} remote get-url origin
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_URL
                OUTPUT_STRIP_TRAILING_WHITESPACE)

add_compile_definitions(
    GIT_VERSION="${GIT_VERSION}"
    GIT_VERSION_DATE="${GIT_VERSION_DATE}"
    GIT_TAG="${GIT_TAG}"
    GIT_URL="${GIT_URL}"
)

message(STATUS "Git information:")
message(STATUS " Tag: ${GIT_TAG}")
message(STATUS " Version: ${GIT_VERSION}")
message(STATUS " Version Date: ${GIT_VERSION_DATE}")
message(STATUS " URL: ${GIT_URL}")
