add_library(
    logger
STATIC
    logger.cpp
    loglistmodel.cpp
)

target_link_libraries(logger
PRIVATE
    Qt5::Concurrent
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    filemanager # for the construtor only, perhaps we should remove that dependency.
)
