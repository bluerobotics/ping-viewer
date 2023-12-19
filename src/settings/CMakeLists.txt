add_library(
    settings
STATIC
    qjsonsettings.cpp
    settingsmanager.cpp
    varianttree.cpp
)

target_link_libraries(
    settings
PRIVATE
    Qt6::Core
    Qt6::Qml
    Qt6::Widgets # because of QColor
)
