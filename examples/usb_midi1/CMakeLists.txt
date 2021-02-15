cmake_minimum_required(VERSION 3.19)

# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
include("${PICO_SDK_PATH}/external/pico_sdk_import.cmake")

# define project settins
set(ARDUINO_SKETCH_NAME "usb_midi" )
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
#set(ARDUINO_USB_MIDI_PATH ${PICO_SDK_ARDUINO_PATH}/Arduino/USB/midi)

# Standard Sketch logic
project("${ARDUINO_SKETCH_NAME}" C CXX ASM)
#file(GLOB USB_SRC ${ARDUINO_USB_MIDI_PATH}/*.cpp )
file(GLOB CURRENT_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
set(ARDUINO_SKETCH_SOURCE  ${CURRENT_SRC} )
set(ARDUINO_SKETCH_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})
set(ARDUINO_SKETCH_LIB "tinyusb_device" "tinyusb_board" "pico_fix_rp2040_usb_device_enumeration"  )

set(CMAKE_C_FLAGS_DEBUG "-Og") 
add_compile_options(-DCFG_TUSB_DEBUG=3 -DCFG_TUSB_MCU=OPT_MCU_RP2040)

include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")


