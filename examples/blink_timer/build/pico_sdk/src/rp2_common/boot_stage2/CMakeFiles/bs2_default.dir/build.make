# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.19

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.19.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.19.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer/build

# Include any dependencies generated for this target.
include pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/depend.make

# Include the progress variables for this target.
include pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/progress.make

# Include the compile flags for this target's objects.
include pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/flags.make

pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/boot2_w25q080.S.obj: pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/flags.make
pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/boot2_w25q080.S.obj: /Users/pschatzmann/Documents/Projects/cpp/pico-sdk/src/rp2_common/boot_stage2/boot2_w25q080.S
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building ASM object pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/boot2_w25q080.S.obj"
	cd /Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer/build/pico_sdk/src/rp2_common/boot_stage2 && /usr/local/bin/arm-none-eabi-gcc $(ASM_DEFINES) $(ASM_INCLUDES) $(ASM_FLAGS) -o CMakeFiles/bs2_default.dir/boot2_w25q080.S.obj   -c /Users/pschatzmann/Documents/Projects/cpp/pico-sdk/src/rp2_common/boot_stage2/boot2_w25q080.S

# Object files for target bs2_default
bs2_default_OBJECTS = \
"CMakeFiles/bs2_default.dir/boot2_w25q080.S.obj"

# External object files for target bs2_default
bs2_default_EXTERNAL_OBJECTS =

pico_sdk/src/rp2_common/boot_stage2/bs2_default.elf: pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/boot2_w25q080.S.obj
pico_sdk/src/rp2_common/boot_stage2/bs2_default.elf: pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/build.make
pico_sdk/src/rp2_common/boot_stage2/bs2_default.elf: /Users/pschatzmann/Documents/Projects/cpp/pico-sdk/src/rp2_common/boot_stage2/boot_stage2.ld
pico_sdk/src/rp2_common/boot_stage2/bs2_default.elf: pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking ASM executable bs2_default.elf"
	cd /Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer/build/pico_sdk/src/rp2_common/boot_stage2 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/bs2_default.dir/link.txt --verbose=$(VERBOSE)
	cd /Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer/build/pico_sdk/src/rp2_common/boot_stage2 && /usr/local/bin/arm-none-eabi-objdump -h bs2_default.elf >bs2_default.dis
	cd /Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer/build/pico_sdk/src/rp2_common/boot_stage2 && /usr/local/bin/arm-none-eabi-objdump -d bs2_default.elf >>bs2_default.dis

# Rule to build all files generated by this target.
pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/build: pico_sdk/src/rp2_common/boot_stage2/bs2_default.elf

.PHONY : pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/build

pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/clean:
	cd /Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer/build/pico_sdk/src/rp2_common/boot_stage2 && $(CMAKE_COMMAND) -P CMakeFiles/bs2_default.dir/cmake_clean.cmake
.PHONY : pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/clean

pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/depend:
	cd /Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer /Users/pschatzmann/Documents/Projects/cpp/pico-sdk/src/rp2_common/boot_stage2 /Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer/build /Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer/build/pico_sdk/src/rp2_common/boot_stage2 /Users/pschatzmann/Documents/Projects/cpp/pico-arduino/examples/blink_timer/build/pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : pico_sdk/src/rp2_common/boot_stage2/CMakeFiles/bs2_default.dir/depend

