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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.22.3/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.22.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/YandievAbdullax/desktop/fla/dfa_minim

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/YandievAbdullax/desktop/fla/dfa_minim

# Include any dependencies generated for this target.
include CMakeFiles/dfa_minim.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/dfa_minim.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/dfa_minim.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/dfa_minim.dir/flags.make

CMakeFiles/dfa_minim.dir/main.cpp.o: CMakeFiles/dfa_minim.dir/flags.make
CMakeFiles/dfa_minim.dir/main.cpp.o: main.cpp
CMakeFiles/dfa_minim.dir/main.cpp.o: CMakeFiles/dfa_minim.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/YandievAbdullax/desktop/fla/dfa_minim/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/dfa_minim.dir/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dfa_minim.dir/main.cpp.o -MF CMakeFiles/dfa_minim.dir/main.cpp.o.d -o CMakeFiles/dfa_minim.dir/main.cpp.o -c /Users/YandievAbdullax/desktop/fla/dfa_minim/main.cpp

CMakeFiles/dfa_minim.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dfa_minim.dir/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/YandievAbdullax/desktop/fla/dfa_minim/main.cpp > CMakeFiles/dfa_minim.dir/main.cpp.i

CMakeFiles/dfa_minim.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dfa_minim.dir/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/YandievAbdullax/desktop/fla/dfa_minim/main.cpp -o CMakeFiles/dfa_minim.dir/main.cpp.s

CMakeFiles/dfa_minim.dir/task.cpp.o: CMakeFiles/dfa_minim.dir/flags.make
CMakeFiles/dfa_minim.dir/task.cpp.o: task.cpp
CMakeFiles/dfa_minim.dir/task.cpp.o: CMakeFiles/dfa_minim.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/YandievAbdullax/desktop/fla/dfa_minim/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/dfa_minim.dir/task.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/dfa_minim.dir/task.cpp.o -MF CMakeFiles/dfa_minim.dir/task.cpp.o.d -o CMakeFiles/dfa_minim.dir/task.cpp.o -c /Users/YandievAbdullax/desktop/fla/dfa_minim/task.cpp

CMakeFiles/dfa_minim.dir/task.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/dfa_minim.dir/task.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/YandievAbdullax/desktop/fla/dfa_minim/task.cpp > CMakeFiles/dfa_minim.dir/task.cpp.i

CMakeFiles/dfa_minim.dir/task.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/dfa_minim.dir/task.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/YandievAbdullax/desktop/fla/dfa_minim/task.cpp -o CMakeFiles/dfa_minim.dir/task.cpp.s

# Object files for target dfa_minim
dfa_minim_OBJECTS = \
"CMakeFiles/dfa_minim.dir/main.cpp.o" \
"CMakeFiles/dfa_minim.dir/task.cpp.o"

# External object files for target dfa_minim
dfa_minim_EXTERNAL_OBJECTS =

dfa_minim: CMakeFiles/dfa_minim.dir/main.cpp.o
dfa_minim: CMakeFiles/dfa_minim.dir/task.cpp.o
dfa_minim: CMakeFiles/dfa_minim.dir/build.make
dfa_minim: CMakeFiles/dfa_minim.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/YandievAbdullax/desktop/fla/dfa_minim/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable dfa_minim"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dfa_minim.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/dfa_minim.dir/build: dfa_minim
.PHONY : CMakeFiles/dfa_minim.dir/build

CMakeFiles/dfa_minim.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/dfa_minim.dir/cmake_clean.cmake
.PHONY : CMakeFiles/dfa_minim.dir/clean

CMakeFiles/dfa_minim.dir/depend:
	cd /Users/YandievAbdullax/desktop/fla/dfa_minim && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/YandievAbdullax/desktop/fla/dfa_minim /Users/YandievAbdullax/desktop/fla/dfa_minim /Users/YandievAbdullax/desktop/fla/dfa_minim /Users/YandievAbdullax/desktop/fla/dfa_minim /Users/YandievAbdullax/desktop/fla/dfa_minim/CMakeFiles/dfa_minim.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/dfa_minim.dir/depend
