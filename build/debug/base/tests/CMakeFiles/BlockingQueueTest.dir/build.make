# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/guang19/Projects/nets

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/guang19/Projects/nets/build/debug

# Include any dependencies generated for this target.
include base/tests/CMakeFiles/BlockingQueueTest.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include base/tests/CMakeFiles/BlockingQueueTest.dir/compiler_depend.make

# Include the progress variables for this target.
include base/tests/CMakeFiles/BlockingQueueTest.dir/progress.make

# Include the compile flags for this target's objects.
include base/tests/CMakeFiles/BlockingQueueTest.dir/flags.make

base/tests/CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.o: base/tests/CMakeFiles/BlockingQueueTest.dir/flags.make
base/tests/CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.o: ../../base/tests/BlockingQueueTest.cpp
base/tests/CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.o: base/tests/CMakeFiles/BlockingQueueTest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/guang19/Projects/nets/build/debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object base/tests/CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.o"
	cd /home/guang19/Projects/nets/build/debug/base/tests && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT base/tests/CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.o -MF CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.o.d -o CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.o -c /home/guang19/Projects/nets/base/tests/BlockingQueueTest.cpp

base/tests/CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.i"
	cd /home/guang19/Projects/nets/build/debug/base/tests && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/guang19/Projects/nets/base/tests/BlockingQueueTest.cpp > CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.i

base/tests/CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.s"
	cd /home/guang19/Projects/nets/build/debug/base/tests && /usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/guang19/Projects/nets/base/tests/BlockingQueueTest.cpp -o CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.s

# Object files for target BlockingQueueTest
BlockingQueueTest_OBJECTS = \
"CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.o"

# External object files for target BlockingQueueTest
BlockingQueueTest_EXTERNAL_OBJECTS =

base/tests/BlockingQueueTest: base/tests/CMakeFiles/BlockingQueueTest.dir/BlockingQueueTest.cpp.o
base/tests/BlockingQueueTest: base/tests/CMakeFiles/BlockingQueueTest.dir/build.make
base/tests/BlockingQueueTest: base/threadpool/libnets_base_threadpool.so
base/tests/BlockingQueueTest: base/libnets_base.so
base/tests/BlockingQueueTest: base/tests/CMakeFiles/BlockingQueueTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/guang19/Projects/nets/build/debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable BlockingQueueTest"
	cd /home/guang19/Projects/nets/build/debug/base/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/BlockingQueueTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
base/tests/CMakeFiles/BlockingQueueTest.dir/build: base/tests/BlockingQueueTest
.PHONY : base/tests/CMakeFiles/BlockingQueueTest.dir/build

base/tests/CMakeFiles/BlockingQueueTest.dir/clean:
	cd /home/guang19/Projects/nets/build/debug/base/tests && $(CMAKE_COMMAND) -P CMakeFiles/BlockingQueueTest.dir/cmake_clean.cmake
.PHONY : base/tests/CMakeFiles/BlockingQueueTest.dir/clean

base/tests/CMakeFiles/BlockingQueueTest.dir/depend:
	cd /home/guang19/Projects/nets/build/debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/guang19/Projects/nets /home/guang19/Projects/nets/base/tests /home/guang19/Projects/nets/build/debug /home/guang19/Projects/nets/build/debug/base/tests /home/guang19/Projects/nets/build/debug/base/tests/CMakeFiles/BlockingQueueTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : base/tests/CMakeFiles/BlockingQueueTest.dir/depend

