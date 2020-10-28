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
    Qt5::Core
    Qt5::Qml
    Qt5::Widgets # because of QColor
)
