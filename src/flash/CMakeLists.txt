file(GLOB LIBCINTELHEXFILES
    ${PROJECT_SOURCE_DIR}/lib/libcintelhex/src/ihex_parse.c
    ${PROJECT_SOURCE_DIR}/lib/libcintelhex/src/ihex_record.c
)

add_library(
    flash
STATIC
    flasher.cpp
    pic-hex.cpp
    ping360bootloaderpacket.cpp
    ping360flasher.cpp
    ping360flashworker.cpp
    ${LIBCINTELHEXFILES}
)

target_link_libraries(
    flash
PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::SerialPort
)
