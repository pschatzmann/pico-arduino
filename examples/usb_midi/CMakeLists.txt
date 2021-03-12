cmake_minimum_required(VERSION 3.19)

# define project paths
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(TINYUSB_SDK_PATH $PICO_SDK_PATH}/lib/tinyusb)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Define PICO for c++ Preprocessor 
add_definitions(-DPICO -DARDUINO)

include("${PICO_SDK_PATH}/external/pico_sdk_import.cmake")
include(ExternalProject)

# Standard Sketch logic
project(usb_midi C CXX ASM)

# initialize the Pico SDK
pico_sdk_init()

# Add tinyusb-cpp
ExternalProject_Add(tinyusb-cpp
    GIT_REPOSITORY "https://github.com/pschatzmann/tinyusb-cpp.git"
    GIT_TAG main
    PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb-cpp
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
)


# We search for the ARDUINO library
find_library(ARDUINO_LIB  
    NAMES "arduino" 
    PATHS "${PICO_SDK_ARDUINO_PATH}/lib"
    NO_CMAKE_FIND_ROOT_PATH
    NO_DEFAULT_PATH
    NO_SYSTEM_ENVIRONMENT_PATH
    NO_CMAKE_SYSTEM_PATH
)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${TINYUSB_SDK_PATH}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/tinyusb-cpp/src
    ${ARDUINO_SKETCH_INCLUDES}
    "${PICO_SDK_ARDUINO_PATH}/Arduino/ArduinoCore-API/api" 
    "${PICO_SDK_ARDUINO_PATH}/Arduino/ArduinoCore-Pico/cores/pico"
    "${PICO_SDK_ARDUINO_PATH}/Arduino"
)

# PICO Executable
add_executable(usb_midi ${CMAKE_CURRENT_SOURCE_DIR}/usb_midi.cpp )

pico_enable_stdio_usb(usb_midi 1)
pico_enable_stdio_uart(usb_midi 0)

add_dependencies(usb_midi tinyusb-cpp)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(usb_midi PRIVATE
    pico_stdio_uart 
    pico_stdlib 
    pico_runtime
    pico_multicore 
    pico_util 
    hardware_pwm 
    hardware_adc
    hardware_clocks
    hardware_spi
    hardware_pio
)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(usb_midi)
add_compile_options(-DCFG_TUSB_DEBUG=3 -DCFG_TUSB_OS=OPT_OS_PICO)


