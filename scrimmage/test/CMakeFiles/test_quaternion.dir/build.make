# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/david/scrimmage/scrimmage/test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/david/scrimmage/scrimmage/test

# Include any dependencies generated for this target.
include CMakeFiles/test_quaternion.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_quaternion.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_quaternion.dir/flags.make

CMakeFiles/test_quaternion.dir/test_quaternion.o: CMakeFiles/test_quaternion.dir/flags.make
CMakeFiles/test_quaternion.dir/test_quaternion.o: test_quaternion.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/david/scrimmage/scrimmage/test/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_quaternion.dir/test_quaternion.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_quaternion.dir/test_quaternion.o -c /home/david/scrimmage/scrimmage/test/test_quaternion.cpp

CMakeFiles/test_quaternion.dir/test_quaternion.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_quaternion.dir/test_quaternion.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/david/scrimmage/scrimmage/test/test_quaternion.cpp > CMakeFiles/test_quaternion.dir/test_quaternion.i

CMakeFiles/test_quaternion.dir/test_quaternion.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_quaternion.dir/test_quaternion.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/david/scrimmage/scrimmage/test/test_quaternion.cpp -o CMakeFiles/test_quaternion.dir/test_quaternion.s

CMakeFiles/test_quaternion.dir/test_quaternion.o.requires:

.PHONY : CMakeFiles/test_quaternion.dir/test_quaternion.o.requires

CMakeFiles/test_quaternion.dir/test_quaternion.o.provides: CMakeFiles/test_quaternion.dir/test_quaternion.o.requires
	$(MAKE) -f CMakeFiles/test_quaternion.dir/build.make CMakeFiles/test_quaternion.dir/test_quaternion.o.provides.build
.PHONY : CMakeFiles/test_quaternion.dir/test_quaternion.o.provides

CMakeFiles/test_quaternion.dir/test_quaternion.o.provides.build: CMakeFiles/test_quaternion.dir/test_quaternion.o


# Object files for target test_quaternion
test_quaternion_OBJECTS = \
"CMakeFiles/test_quaternion.dir/test_quaternion.o"

# External object files for target test_quaternion
test_quaternion_EXTERNAL_OBJECTS =

test_quaternion: CMakeFiles/test_quaternion.dir/test_quaternion.o
test_quaternion: CMakeFiles/test_quaternion.dir/build.make
test_quaternion: CMakeFiles/test_quaternion.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/david/scrimmage/scrimmage/test/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_quaternion"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_quaternion.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_quaternion.dir/build: test_quaternion

.PHONY : CMakeFiles/test_quaternion.dir/build

CMakeFiles/test_quaternion.dir/requires: CMakeFiles/test_quaternion.dir/test_quaternion.o.requires

.PHONY : CMakeFiles/test_quaternion.dir/requires

CMakeFiles/test_quaternion.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_quaternion.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_quaternion.dir/clean

CMakeFiles/test_quaternion.dir/depend:
	cd /home/david/scrimmage/scrimmage/test && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/david/scrimmage/scrimmage/test /home/david/scrimmage/scrimmage/test /home/david/scrimmage/scrimmage/test /home/david/scrimmage/scrimmage/test /home/david/scrimmage/scrimmage/test/CMakeFiles/test_quaternion.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_quaternion.dir/depend
