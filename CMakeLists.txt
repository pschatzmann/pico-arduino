cmake_minimum_required(VERSION 3.12)

# Initialization 
set(PICO_SDK_ARDUINO_PATH ${CMAKE_CURRENT_SOURCE_DIR})

if (NOT PICO_SDK_PATH)
    if (DEFINED ENV{PICO_SDK_PATH})
        set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
        message("Using PICO_SDK_PATH from environment ('${PICO_SDK_PATH}')")
    else ()
        message(FATAL_ERROR "PICO_SDK_PATH was not found")
    endif ()
endif ()

project(pico-arduino)

add_subdirectory(Arduino)
add_subdirectory(examples EXCLUDE_FROM_ALL)



