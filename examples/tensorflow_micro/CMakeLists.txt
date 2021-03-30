cmake_minimum_required(VERSION 3.19.2)

# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_NAME "tensorflow_micro" )

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(FetchContent)

project("${ARDUINO_SKETCH_NAME}")

## Download Tensorflow from Guthub
FetchContent_Declare( tf  GIT_REPOSITORY "https://github.com/pschatzmann/pico-tflmicro.git" GIT_TAG main )
FetchContent_MakeAvailable(tf)

## Define the Source file and library
set(ARDUINO_SKETCH_SOURCE tf_hallo_world.cpp  )
set(ARDUINO_SKETCH_LIB  pico-tflmicro)

include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")

set_target_properties(
  "${ARDUINO_SKETCH_NAME}"
  PROPERTIES
  COMPILE_FLAGS -fno-rtti
  COMPILE_FLAGS -fno-exceptions
  COMPILE_FLAGS -fno-threadsafe-statics
  COMPILE_FLAGS -nostdlib
)

