add_library(
    notification
STATIC
    notificationmanager.cpp
    notificationmodel.cpp
)

target_link_libraries(
    notification
PRIVATE
    Qt5::Core
    Qt5::Widgets # because of stylemanager.h
    Qt5::Quick # because of stylemanager.h
)
