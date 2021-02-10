cmake_minimum_required(VERSION 3.12)

# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})

# define project settins
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(USB_PATH ${PICO_SDK_ARDUINO_PATH}/Arduino/USB )
set(ARDUINO_SKETCH_NAME "usb_dev_audio" )
set(ARDUINO_SKETCH_SOURCE 
    "usb_dev_audio.cpp" 
    "${USB_PATH}/dev_audio_headset_cb.cpp" 
    "${USB_PATH}/usb_descriptors.c"  
)
set(ARDUINO_SKETCH_LIB "tinyusb_device" "tinyusb_board"  )
include("${PICO_SDK_PATH}/external/pico_sdk_import.cmake")

# Standard Sketch logic
project("${ARDUINO_SKETCH_NAME}" C CXX ASM)
set(CMAKE_C_FLAGS_DEBUG "-O0 -g")
include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")


