cmake_minimum_required(VERSION 3.16)
project(IridiumProject)

enable_language(C)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(BUILD_PYTHON_SHARED_LIBRARY "Set to ON to enable building of python shared library" OFF)

if(DEFINED DEBUG AND DEBUG STREQUAL "ON")
    add_definitions(-DDEBUG)
endif()

# Check for libgpiod
find_library(LIB_GPIOD gpiod)

if (NOT LIB_GPIOD)
    message(STATUS "GPIO support is DISABLED (libgpiod not found)")
    set(GPIO_ENABLED FALSE CACHE BOOL "Set if libgpiod is available")
else()
    message(STATUS "GPIO support is ENABLED (libgpiod found)")
    set(GPIO_ENABLED TRUE CACHE BOOL "Set if libgpiod is available")
    add_compile_definitions(RB_GPIO)
endif()

if(WIN32)
    message(STATUS "Building on Windows")
    set(FW_UPDATE_DEFAULT ON)
    add_compile_definitions(-D_CRT_SECURE_NO_WARNINGS)
elseif(APPLE)
    message(STATUS "Building on macOS")
    set(FW_UPDATE_DEFAULT ON)
elseif(UNIX AND NOT APPLE)
    message(STATUS "Building on Linux")
    set(FW_UPDATE_DEFAULT ON)
else()
    set(FW_UPDATE_DEFAULT OFF)
endif()

option(FW_UPDATE "Enable Kermit firmware update" ${FW_UPDATE_DEFAULT})

# Source directories
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(EXAMPLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/examples)
set(THIRD_PARTY_DIR ${SRC_DIR}/third_party)
set(KERMIT_DIR ${THIRD_PARTY_DIR}/ekermit)
set(KERMIT_IO_DIR ${SRC_DIR}/kermit)
set(WINDOWS_GET_OPT_DIR ${THIRD_PARTY_DIR}/wingetopt/src)
set(PYTHON_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/rockblock9704)

# Library build here
set(KERMIT_LIB kermit)
set(WINDOWS_GET_OPT_LIB wingetopt)
set(IRIDIUM_IMT_LIB iridiumImt)
set(PYTHON_SHARED_LIB rockblock)

# Some parent variables so projects can link against us and include us
get_directory_property(HAS_PARENT PARENT_DIRECTORY)
if (HAS_PARENT)
    set(RB9704_INCLUDES ${SRC_DIR} PARENT_SCOPE)
    set(RB9704_LIB ${IRIDIUM_IMT_LIB} PARENT_SCOPE)
endif()


# Example binaries
set(REFLECTED_BIN reflected)
set(CL_RAW_BIN cloudloopRaw)
set(HARDWARE_INFO_BIN hardwareInfo)
set(CUSTOM_MESSAGE_BIN customMessage)
set(CUSTOM_FILE_BIN customFileMessage)
set(FW_UPDATE_BIN firmwareUpdate)
set(GPIO_FILE_BIN gpioPiHat)
set(GPIO_CUSTOM_BIN gpioCustom)
set(ASYNC_SEND_RECEIVE_BIN asyncSendReceive)

include_directories(${SRC_DIR}
    ${KERMIT_DIR}
    ${KERMIT_IO_DIR})

add_subdirectory(${SRC_DIR})

if (GPIO_ENABLED)
    set(GPIO_SRC gpio.c)
else()
    set(GPIO_SRC "")
endif()

if (BUILD_PYTHON_SHARED_LIBRARY STREQUAL "ON")
    find_package (Python3 COMPONENTS Interpreter Development.Module REQUIRED)

    add_library(${PYTHON_SHARED_LIB} SHARED
        ${SRC_DIR}/rockblock_9704.c
        ${SRC_DIR}/jspr_command.c
        ${SRC_DIR}/jspr.c
        ${SRC_DIR}/${GPIO_SRC}
        ${SRC_DIR}/crossplatform.c
        ${SRC_DIR}/serial.c
        ${SRC_DIR}/serial_presets/serial_linux/serial_linux.c
        ${SRC_DIR}/serial_presets/serial_windows/serial_windows.c
        ${SRC_DIR}/serial_presets/serial_arduino/serial_arduino.cpp
        ${PYTHON_SRC_DIR}/rockblock.c
    )
    target_include_directories(${PYTHON_SHARED_LIB} PRIVATE ${Python3_INCLUDE_DIRS} ${IRIDIUM_IMT_LIB})
    target_link_libraries(${PYTHON_SHARED_LIB} PRIVATE ${Python3_LIBRARIES} ${IRIDIUM_IMT_LIB})
    if(APPLE)
        target_link_options(${PYTHON_SHARED_LIB} PRIVATE "-undefined" "dynamic_lookup")
    endif()

    set_target_properties(${PYTHON_SHARED_LIB} PROPERTIES OUTPUT_NAME ${PYTHON_SHARED_LIB})
endif()

add_executable(${REFLECTED_BIN} ${EXAMPLE_DIR}/reflected.c)
add_executable(${CL_RAW_BIN} ${EXAMPLE_DIR}/cloudloopRaw.c)
add_executable(${HARDWARE_INFO_BIN} ${EXAMPLE_DIR}/hardwareInfo.c)
add_executable(${CUSTOM_MESSAGE_BIN} ${EXAMPLE_DIR}/customMessage.c)
add_executable(${CUSTOM_FILE_BIN} ${EXAMPLE_DIR}/customFileMessage.c)
add_executable(${ASYNC_SEND_RECEIVE_BIN} ${EXAMPLE_DIR}/asyncSendReceive.c)

target_include_directories(${REFLECTED_BIN} PRIVATE ${SRC_DIR})
target_include_directories(${CL_RAW_BIN} PRIVATE ${SRC_DIR})
target_include_directories(${HARDWARE_INFO_BIN} PRIVATE ${SRC_DIR})
target_include_directories(${CUSTOM_MESSAGE_BIN} PRIVATE ${SRC_DIR})
target_include_directories(${CUSTOM_FILE_BIN} PRIVATE ${SRC_DIR})
target_include_directories(${ASYNC_SEND_RECEIVE_BIN} PRIVATE ${SRC_DIR})


if (GPIO_ENABLED)
    add_executable(${GPIO_FILE_BIN} ${EXAMPLE_DIR}/gpioPiHat.c)
    target_include_directories(${GPIO_FILE_BIN} PRIVATE ${SRC_DIR})
    target_link_libraries(${GPIO_FILE_BIN} PRIVATE ${IRIDIUM_IMT_LIB})
    add_executable(${GPIO_CUSTOM_BIN} ${EXAMPLE_DIR}/gpioCustom.c)
    target_include_directories(${GPIO_CUSTOM_BIN} PRIVATE ${SRC_DIR})
    target_link_libraries(${GPIO_CUSTOM_BIN} PRIVATE ${IRIDIUM_IMT_LIB})
endif()

if(WIN32)
    add_library(${WINDOWS_GET_OPT_LIB} ${WINDOWS_GET_OPT_DIR}/getopt.c)
    include_directories(${WINDOWS_GET_OPT_DIR})

    target_link_libraries(${REFLECTED_BIN} PRIVATE ${IRIDIUM_IMT_LIB} ${WINDOWS_GET_OPT_LIB})
    target_link_libraries(${CL_RAW_BIN} PRIVATE ${IRIDIUM_IMT_LIB} ${WINDOWS_GET_OPT_LIB})
    target_link_libraries(${HARDWARE_INFO_BIN} PRIVATE ${IRIDIUM_IMT_LIB} ${WINDOWS_GET_OPT_LIB})
    target_link_libraries(${CUSTOM_MESSAGE_BIN} PRIVATE ${IRIDIUM_IMT_LIB} ${WINDOWS_GET_OPT_LIB})
    target_link_libraries(${CUSTOM_FILE_BIN} PRIVATE ${IRIDIUM_IMT_LIB} ${WINDOWS_GET_OPT_LIB})
    target_link_libraries(${ASYNC_SEND_RECEIVE_BIN} PRIVATE ${IRIDIUM_IMT_LIB} ${WINDOWS_GET_OPT_LIB})
else()
    target_link_libraries(${REFLECTED_BIN} PRIVATE ${IRIDIUM_IMT_LIB})
    target_link_libraries(${CL_RAW_BIN} PRIVATE ${IRIDIUM_IMT_LIB})
    target_link_libraries(${HARDWARE_INFO_BIN} PRIVATE ${IRIDIUM_IMT_LIB})
    target_link_libraries(${CUSTOM_MESSAGE_BIN} PRIVATE ${IRIDIUM_IMT_LIB})
    target_link_libraries(${CUSTOM_FILE_BIN} PRIVATE ${IRIDIUM_IMT_LIB})
    target_link_libraries(${ASYNC_SEND_RECEIVE_BIN} PRIVATE ${IRIDIUM_IMT_LIB})
endif()

if (DEFINED FW_UPDATE AND FW_UPDATE STREQUAL "ON")
    add_compile_definitions(KERMIT)
    add_library(${KERMIT_LIB} ${KERMIT_IO_DIR}/kermit_io.c ${KERMIT_DIR}/kermit.c)
    target_include_directories(${KERMIT_LIB} PRIVATE ${SRC_DIR} ${KERMIT_DIR})

    add_executable(${FW_UPDATE_BIN} ${EXAMPLE_DIR}/firmware-update.c)

    target_include_directories(${FW_UPDATE_BIN} PRIVATE ${SRC_DIR} ${KERMIT_IO_DIR})

    if(WIN32)
        target_link_libraries(${FW_UPDATE_BIN} PRIVATE ${IRIDIUM_IMT_LIB} ${KERMIT_LIB} ${WINDOWS_GET_OPT_LIB})
    else()
        target_link_libraries(${FW_UPDATE_BIN} PRIVATE ${IRIDIUM_IMT_LIB} ${KERMIT_LIB})
    endif()
endif()

# Doxygen documentation generation
find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rockblock-9704-doxygen.config")

    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
else()
    message(STATUS "Doxygen not found. 'make docs' target will not be available.")
endif()