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
set(LUA_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lua)

# Library build here
set(KERMIT_LIB kermit)
set(WINDOWS_GET_OPT_LIB wingetopt)
set(IRIDIUM_IMT_LIB iridiumImt)
set(PYTHON_SHARED_LIB rockblock)
set(LUA_SHARED_LIB rockblock_lua) # Lua shared library name This will result in rockblock_lua.so/.dll/.dylib

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





if (BUILD_LUA_SHARED_LIBRARY STREQUAL "ON")
    # TODO - MAKE THIS WORK RATHER THAN HARD CODING
    #find_package(Lua54 REQUIRED)

    # Path to the directory containing Lua header files (e.g., lua.h, lauxlib.h)
    set(Lua54_INCLUDE_DIR "C:/Users/Joel/Documents/Lua/lua/include")
    message(STATUS "Hardcoding Lua Include Dir: ${Lua54_INCLUDE_DIR}")

    # Path to the Lua library file (e.g., lua54.lib for MSVC)
    set(Lua54_LIBRARIES "C:/Users/Joel/Documents/Lua/lua/bin/lua54.lib")
    message(STATUS "Hardcoding Lua Libraries: ${Lua54_LIBRARIES}")

    # This boolean is usually set by find_package. Manually set it to TRUE to proceed.
    set(Lua54_FOUND TRUE)

    # Ensure Lua was "found" (or manually marked as found)
    if(NOT Lua54_FOUND)
        message(FATAL_ERROR "Lua 5.4 was not found, even with hardcoded paths. Please check your paths.")
    endif()


    # Define the shared library for Lua
    add_library(${LUA_SHARED_LIB} SHARED
        ${SRC_DIR}/rockblock_9704.c
        ${SRC_DIR}/jspr_command.c
        ${SRC_DIR}/jspr.c
        ${SRC_DIR}/${GPIO_SRC}
        ${SRC_DIR}/crossplatform.c
        ${SRC_DIR}/serial.c
        ${SRC_DIR}/serial_presets/serial_linux/serial_linux.c
        ${SRC_DIR}/serial_presets/serial_windows/serial_windows.c
        ${SRC_DIR}/serial_presets/serial_arduino/serial_arduino.cpp
        ${LUA_SRC_DIR}/rockblock_lua_wrap.cxx # Path to generated SWIG file
    )

    target_include_directories(${LUA_SHARED_LIB} PRIVATE
        ${Lua54_INCLUDE_DIR}
        ${IRIDIUM_IMT_LIB}
        ${CMAKE_CURRENT_SOURCE_DIR}
    )

    target_link_libraries(${LUA_SHARED_LIB} PRIVATE
        ${Lua54_LIBRARIES}
        ${IRIDIUM_IMT_LIB}
    )

    if(APPLE)
        target_link_options(${LUA_SHARED_LIB} PRIVATE "-undefined" "dynamic_lookup")
    endif()

    set_target_properties(${LUA_SHARED_LIB} PROPERTIES
        OUTPUT_NAME ${LUA_SHARED_LIB} # Sets the base name
        # SUFFIX ".so" # Linux/macOS suffix (CMake handles .dll for Windows automatically for SHARED libraries)
                      # SWIG will likely use 'rockblock_lua.so' for a module named 'rockblock_lua'.
                      # Check SWIG's output naming conventions carefully.
                      # If SWIG output is always rockblock_lua.so, you'd name your add_library target 'rockblock_lua'.
    )
endif()

add_executable(${REFLECTED_BIN} ${EXAMPLE_DIR}/reflected.c)
add_executable(${CL_RAW_BIN} ${EXAMPLE_DIR}/cloudloopRaw.c)
add_executable(${HARDWARE_INFO_BIN} ${EXAMPLE_DIR}/hardwareInfo.c)
add_executable(${CUSTOM_MESSAGE_BIN} ${EXAMPLE_DIR}/customMessage.c)
add_executable(${CUSTOM_FILE_BIN} ${EXAMPLE_DIR}/customFileMessage.c)

target_include_directories(${REFLECTED_BIN} PRIVATE ${SRC_DIR})
target_include_directories(${CL_RAW_BIN} PRIVATE ${SRC_DIR})
target_include_directories(${HARDWARE_INFO_BIN} PRIVATE ${SRC_DIR})
target_include_directories(${CUSTOM_MESSAGE_BIN} PRIVATE ${SRC_DIR})
target_include_directories(${CUSTOM_FILE_BIN} PRIVATE ${SRC_DIR})


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
else()
    target_link_libraries(${REFLECTED_BIN} PRIVATE ${IRIDIUM_IMT_LIB})
    target_link_libraries(${CL_RAW_BIN} PRIVATE ${IRIDIUM_IMT_LIB})
    target_link_libraries(${HARDWARE_INFO_BIN} PRIVATE ${IRIDIUM_IMT_LIB})
    target_link_libraries(${CUSTOM_MESSAGE_BIN} PRIVATE ${IRIDIUM_IMT_LIB})
    target_link_libraries(${CUSTOM_FILE_BIN} PRIVATE ${IRIDIUM_IMT_LIB})
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