cmake_minimum_required(VERSION 3.12)
# initialize PICO_SDK_PATH
if (DEFINED ENV{PICO_SDK_PATH} AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
    message("Using PICO_SDK_PATH from environment ('${PICO_SDK_PATH}')")
endif ()

# DEFINED PICO_SDK_ARDUINO_PATH
if (DEFINED ENV{PICO_SDK_ARDUINO_PATH} AND (NOT PICO_SDK_ARDUINO_PATH))
    set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
    message("Using PICO_SDK_ARDUINO_PATH from environment ('${PICO_SDK_ARDUINO_PATH}')")
endif ()

# Initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# The Project starts here
project(ArduinoPicoLibrary)

# We add some additional DEFINE
add_definitions(-DPICO -DARDUINO)

# initialize the Pico SDK
pico_sdk_init()

# Arduino PICO Implementation
file(GLOB ARD_DIR_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/ArduinoCore-API/api/*.cpp" )
file(GLOB ARD_DIR_EXT_LIST CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/ArduinoCore-Pico/*.cpp" )

add_library(arduino ${ARD_DIR_LIST} ${ARD_DIR_EXT_LIST} )

target_include_directories(use_as_library PUBLIC
    "${PICO_SDK_ARDUINO_PATH}/Arduino"
    "${PICO_SDK_ARDUINO_PATH}/Arduino/ArduinoCore-API/api" 
    "${PICO_SDK_ARDUINO_PATH}/Arduino/ArduinoCore-Pico"
)

target_link_libraries(arduino
    pico_stdio_usb 
    pico_stdlib 
    pico_runtime
    pico_multicore 
    pico_util 
    pico_unique_id
    hardware_pwm 
    hardware_adc
    hardware_clocks
    hardware_spi
    hardware_i2c
    hardware_pio
    hardware_dma
)

install(TARGETS arduino
    DESTINATION ${PICO_SDK_ARDUINO_PATH}/lib
)


