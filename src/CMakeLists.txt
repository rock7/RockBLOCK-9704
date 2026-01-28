if (GPIO_ENABLED)
    set(GPIO_SRC gpio.c)
else()
    set(GPIO_SRC "")
endif()

add_library(
    ${IRIDIUM_IMT_LIB}
    rockblock_9704.c
    jspr.c
    jspr_command.c
    serial.c
    imt_queue.c
    ${GPIO_SRC}
    crossplatform.c
    serial_presets/serial_linux/serial_linux.c
    serial_presets/serial_windows/serial_windows.c
    serial_presets/serial_arduino/serial_arduino.cpp
    ${THIRD_PARTY_DIR}/cJSON/cJSON.c
    ${THIRD_PARTY_DIR}/base64/base64.c)

if (GPIO_ENABLED)
    target_link_libraries(${IRIDIUM_IMT_LIB} PUBLIC ${LIB_GPIOD})
endif()

target_include_directories(${IRIDIUM_IMT_LIB} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (DEFINED FW_UPDATE AND FW_UPDATE STREQUAL "ON")
    target_compile_definitions(${IRIDIUM_IMT_LIB} PUBLIC KERMIT)
    target_link_libraries(${IRIDIUM_IMT_LIB} PUBLIC ${KERMIT_LIB})
    target_include_directories(${IRIDIUM_IMT_LIB} PUBLIC ${KERMIT_DIR} ${KERMIT_IO_DIR})
endif()
