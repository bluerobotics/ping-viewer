add_library(
    qquickcomponents
STATIC
    qquickopengl.cpp
)

target_link_libraries(qquickcomponents
PRIVATE
    Qt5::Core
    Qt5::Quick
)
