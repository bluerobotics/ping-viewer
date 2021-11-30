add_library(
    logger
STATIC
    logger.cpp
    loglistmodel.cpp
)

target_link_libraries(
    logger
PRIVATE
    Qt6::Concurrent
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    filemanager # for the construtor only, perhaps we should remove that dependency.
)
