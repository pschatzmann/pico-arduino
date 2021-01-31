###
#  Generic makefile for Arduino Sketches you need to set the following parameters
#  - ARDUINO_SKETCH_NAME
#  - PICO_SDK_ARDUINO_PATH

###

# initialize PICO_SDK_PATH
if (DEFINED ENV{PICO_SDK_PATH} AND (NOT PICO_SDK_PATH))
    set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
    message("Using PICO_SDK_PATH from environment ('${PICO_SDK_PATH}')")
endif ()
# initialize the Pico SDK
pico_sdk_init()

# DEFINED PICO_SDK_ARDUINO_PATH
if (DEFINED ENV{PICO_SDK_ARDUINO_PATH} AND (NOT PICO_SDK_ARDUINO_PATH))
    set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
    message("Using PICO_SDK_ARDUINO_PATH from environment ('${PICO_SDK_ARDUINO_PATH}')")
endif ()

# Define PICO for c++ Preprocessor 
add_definitions(-DPICO)

# We search for the ARDUINO library
find_library(ARDUINO_LIB  
    NAMES "arduino" 
    PATHS "${PICO_SDK_ARDUINO_PATH}/lib"
    NO_CMAKE_FIND_ROOT_PATH
    NO_DEFAULT_PATH
    NO_SYSTEM_ENVIRONMENT_PATH
    NO_CMAKE_SYSTEM_PATH
)
include_directories("${PICO_SDK_ARDUINO_PATH}/Arduino/ArduinoCore-API/api" "${PICO_SDK_ARDUINO_PATH}/Arduino/ArduinoCore-Pico/cores/pico")

# PICO
add_executable("${ARDUINO_SKETCH_NAME}" ${HEADER_LIST} "${ARDUINO_SKETCH_SOURCE}" )
pico_enable_stdio_usb("${ARDUINO_SKETCH_NAME}" 1)
pico_enable_stdio_uart("${ARDUINO_SKETCH_NAME}" 1)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries("${ARDUINO_SKETCH_NAME}" 
    ${ARDUINO_LIB}
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
)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs("${ARDUINO_SKETCH_NAME}")
