
cmake_minimum_required(VERSION 3.19.2)
# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(TINYUSB_SDK_PATH $PICO_SDK_PATH}/lib/tinyusb)
set(ARDUINO_SKETCH_NAME "usb_midi" )

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project("${ARDUINO_SKETCH_NAME}")

## Define all Source files incl SdFat
set(ARDUINO_SKETCH_SOURCE  "usb_midi.cpp" "usb_main.cpp" )
set(ARDUINO_SKETCH_INCLUDES  ${TINYUSB_SDK_PATH}/src)
set(ARDUINO_SKETCH_LIB "pico_stdio_usb" "tinyusb_device" "tinyusb_board" )
set(ARDUINO_COMPILE_OPTIONS  -DCFG_TUSB_DEBUG=3 -DCFG_TUSB_OS=OPT_OS_PICO)

include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")


