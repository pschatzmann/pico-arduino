cmake_minimum_required(VERSION 3.12)
# PICO initialization
set(PICO_SDK_PATH $ENV{PICO_SDK_PATH})
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Standard Sketch logic
project(all_examples)

# buil all exampleles
add_subdirectory(blink)
add_subdirectory(blink_timer)
add_subdirectory(example_with_arduino_library)
add_subdirectory(hallo_world)
add_subdirectory(hallo_world_serial)
add_subdirectory(i2c_hw290)
add_subdirectory(multi_processor)
add_subdirectory(multi_processor_fifo)
add_subdirectory(queue)
add_subdirectory(sd_info)
add_subdirectory(semaphore)
add_subdirectory(software_serial)
add_subdirectory(tone_melody)
add_subdirectory(usb_midi)
add_subdirectory(hc_05)
