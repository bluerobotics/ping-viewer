add_library(
    notification
STATIC
    notificationmanager.cpp
    notificationmodel.cpp
)

target_link_libraries(
    notification
PRIVATE
    Qt6::Core
    Qt6::Widgets # because of stylemanager.h
    Qt6::Quick # because of stylemanager.h
)
