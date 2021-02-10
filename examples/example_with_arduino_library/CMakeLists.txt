cmake_minimum_required(VERSION 3.12)

# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# define project settins
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})

# Standard Sketch logic
project(example_example_sketch)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Pico SDK
pico_sdk_init()

# Define PICO for c++ Preprocessor 
add_definitions(-DPICO -DARDUINO)

# We search for the Pico ARDUINO library
find_library(ARDUINO_LIB  
    NAMES "arduino" 
    PATHS "${PICO_SDK_ARDUINO_PATH}/lib"
    NO_CMAKE_FIND_ROOT_PATH
    NO_DEFAULT_PATH
    NO_SYSTEM_ENVIRONMENT_PATH
    NO_CMAKE_SYSTEM_PATH
)

# Download Arduino Library
set (git_cmd "clone")
set (git_url "https://github.com/pschatzmann/SpektrumSatellite.git")
execute_process(
    COMMAND git ${git_cmd} ${git_url}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#    COMMAND_ERROR_IS_FATAL ANY
)

# Collect source files in variales
file(GLOB SPEKTRUM_SRC ${CMAKE_CURRENT_SOURCE_DIR}/lib/SpektrumSatellite/src/*.cpp )
file(GLOB CURRENT_SRC  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )

# Define include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/SpektrumSatellite/src
    "${PICO_SDK_ARDUINO_PATH}/Arduino/ArduinoCore-API/api" 
    "${PICO_SDK_ARDUINO_PATH}/Arduino/ArduinoCore-Pico/cores/pico"
    "${PICO_SDK_ARDUINO_PATH}/Arduino"
)


# PICO executable
add_executable(example_sketch ${CURRENT_SRC} ${SPEKTRUM_SRC} )
pico_enable_stdio_usb(example_sketch 1)
pico_enable_stdio_uart(example_sketch 0)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(example_sketch PRIVATE
    "${ARDUINO_LIB}"
    pico_stdio_uart 
    pico_stdio_usb 
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
pico_add_extra_outputs(example_sketch)

