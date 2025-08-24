add_library(
    filemanager
STATIC
    filemanager.cpp
)

target_link_libraries(
    filemanager
PRIVATE
    Qt6::Core
    Qt6::Quick
    logger
)