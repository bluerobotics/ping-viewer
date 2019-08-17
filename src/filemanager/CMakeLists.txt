add_library(
    filemanager
STATIC
    filemanager.cpp
)

target_link_libraries(
    filemanager
PRIVATE
    Qt5::Core
    Qt5::Quick
    logger
)