###
#  Generic makefile for Arduino Sketches you need to set the following parameters
#  - ARDUINO_SKETCH_NAME
#  - PICO_SDK_ARDUINO_PATH
#  There are not dependecies to the arduino library.

###
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

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
add_definitions(-DPICO -DARDUINO=183 -DARDUINO_ARCH_RP2040 ${ARDUINO_SKETCH_DEFINITIONS})

include_directories(
    "${CMAKE_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}"
    ${ARDUINO_SKETCH_INCLUDES}
    ${CMAKE_INSTALL_INCLUDEDIR}
    "${PICO_SDK_ARDUINO_PATH}/Arduino/ArduinoCore-API/api" 
    "${PICO_SDK_ARDUINO_PATH}/Arduino/ArduinoCore-Pico"
    "${PICO_SDK_ARDUINO_PATH}/Arduino"
)

# Arduino PICO Implementation
file(GLOB ARD_DIR_LIST CONFIGURE_DEPENDS "${PICO_SDK_ARDUINO_PATH}/Arduino/ArduinoCore-API/api/*.cpp" )
file(GLOB ARD_DIR_PICO_LIST CONFIGURE_DEPENDS "${PICO_SDK_ARDUINO_PATH}/Arduino/ArduinoCore-Pico/*.cpp" )

# PICO
add_executable(${ARDUINO_SKETCH_NAME} ${HEADER_LIST} ${ARDUINO_SKETCH_SOURCE} ${ARD_DIR_LIST} ${ARD_DIR_PICO_LIST})
#pico_enable_stdio_usb("${ARDUINO_SKETCH_NAME}" 1)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries("${ARDUINO_SKETCH_NAME}" PRIVATE
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
    ${ARDUINO_SKETCH_LIB}
)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs("${ARDUINO_SKETCH_NAME}")
