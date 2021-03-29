cmake_minimum_required(VERSION 3.12)

# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(cmake/GitUtils.cmake) 

# define project settins
set(PICO_SDK_ARDUINO_PATH $ENV{PICO_SDK_ARDUINO_PATH})
set(ARDUINO_SKETCH_NAME "drone" )

## Download of  Arduino library from Guthub
git_clone(PROJECT_NAME dronedyi GIT_URL https://github.com/liourej/CodeDroneDIY.git CONFIGURE_COMMAND "" BUILD_COMMAND "")
## patch MotorsSpeedControl.cpp
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/MotorsSpeedControl.cpp DESTINATION ${dronedyi_SOURCE_DIR}/src/stabilization/hardware )

## Define all Source files 
file(GLOB_RECURSE LIB_SRC "${dronedyi_SOURCE_DIR}/src/*.cpp")
set(ARDUINO_SKETCH_SOURCE ${LIB_SRC} )
set(ARDUINO_SKETCH_DEFINITIONS "-DI2CDEV_IMPLEMENTATION=I2CDEV_ARDUINO_WIRE" "-DBUFFER_LENGTH=512" "-DLOG_DEBUG" "-DNO_WATCHDOG")

# Standard Sketch logic
project("${ARDUINO_SKETCH_NAME}")
include("${PICO_SDK_ARDUINO_PATH}/ArduinoSketch.cmake")

