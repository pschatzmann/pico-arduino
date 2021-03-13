cmake_minimum_required(VERSION 3.19.2)
# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_NAME "spektrum_example" )

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(FetchContent)

project("${ARDUINO_SKETCH_NAME}")

## Download of spektrum_satellite library from Guthub
FetchContent_Declare( spektrum_satellite GIT_REPOSITORY https://github.com/pschatzmann/SpektrumSatellite.git  GIT_TAG  master)
FetchContent_MakeAvailable(spektrum_satellite)

## Define all Source files incl spektrum_satellite
file(GLOB LIB_SRC "${spektrum_satellite_SOURCE_DIR}/src/*.cpp")
file(GLOB CURRENT_SRC  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
set(ARDUINO_SKETCH_SOURCE ${CURRENT_SRC} ${LIB_SRC})
set(ARDUINO_SKETCH_INCLUDES "${spektrum_satellite_SOURCE_DIR}/src/")

include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")


