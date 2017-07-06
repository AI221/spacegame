# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

# Default target executed when no arguments are given to make.
default_target: all

.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:


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
CMAKE_SOURCE_DIR = /home/jackson/spacegame

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jackson/spacegame

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache

.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache

.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/jackson/spacegame/CMakeFiles /home/jackson/spacegame/CMakeFiles/progress.marks
	$(MAKE) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/jackson/spacegame/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean

.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named Spacegame

# Build rule for target.
Spacegame: cmake_check_build_system
	$(MAKE) -f CMakeFiles/Makefile2 Spacegame
.PHONY : Spacegame

# fast build rule for target.
Spacegame/fast:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/build
.PHONY : Spacegame/fast

src/UI.o: src/UI.cpp.o

.PHONY : src/UI.o

# target to build an object file
src/UI.cpp.o:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/UI.cpp.o
.PHONY : src/UI.cpp.o

src/UI.i: src/UI.cpp.i

.PHONY : src/UI.i

# target to preprocess a source file
src/UI.cpp.i:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/UI.cpp.i
.PHONY : src/UI.cpp.i

src/UI.s: src/UI.cpp.s

.PHONY : src/UI.s

# target to generate assembly for a file
src/UI.cpp.s:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/UI.cpp.s
.PHONY : src/UI.cpp.s

src/camera.o: src/camera.cpp.o

.PHONY : src/camera.o

# target to build an object file
src/camera.cpp.o:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/camera.cpp.o
.PHONY : src/camera.cpp.o

src/camera.i: src/camera.cpp.i

.PHONY : src/camera.i

# target to preprocess a source file
src/camera.cpp.i:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/camera.cpp.i
.PHONY : src/camera.cpp.i

src/camera.s: src/camera.cpp.s

.PHONY : src/camera.s

# target to generate assembly for a file
src/camera.cpp.s:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/camera.cpp.s
.PHONY : src/camera.cpp.s

src/debugUI.o: src/debugUI.cpp.o

.PHONY : src/debugUI.o

# target to build an object file
src/debugUI.cpp.o:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/debugUI.cpp.o
.PHONY : src/debugUI.cpp.o

src/debugUI.i: src/debugUI.cpp.i

.PHONY : src/debugUI.i

# target to preprocess a source file
src/debugUI.cpp.i:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/debugUI.cpp.i
.PHONY : src/debugUI.cpp.i

src/debugUI.s: src/debugUI.cpp.s

.PHONY : src/debugUI.s

# target to generate assembly for a file
src/debugUI.cpp.s:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/debugUI.cpp.s
.PHONY : src/debugUI.cpp.s

src/main.o: src/main.cpp.o

.PHONY : src/main.o

# target to build an object file
src/main.cpp.o:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/main.cpp.o
.PHONY : src/main.cpp.o

src/main.i: src/main.cpp.i

.PHONY : src/main.i

# target to preprocess a source file
src/main.cpp.i:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/main.cpp.i
.PHONY : src/main.cpp.i

src/main.s: src/main.cpp.s

.PHONY : src/main.s

# target to generate assembly for a file
src/main.cpp.s:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/main.cpp.s
.PHONY : src/main.cpp.s

src/physics.o: src/physics.cpp.o

.PHONY : src/physics.o

# target to build an object file
src/physics.cpp.o:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/physics.cpp.o
.PHONY : src/physics.cpp.o

src/physics.i: src/physics.cpp.i

.PHONY : src/physics.i

# target to preprocess a source file
src/physics.cpp.i:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/physics.cpp.i
.PHONY : src/physics.cpp.i

src/physics.s: src/physics.cpp.s

.PHONY : src/physics.s

# target to generate assembly for a file
src/physics.cpp.s:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/physics.cpp.s
.PHONY : src/physics.cpp.s

src/renderedPhysicsObject.o: src/renderedPhysicsObject.cpp.o

.PHONY : src/renderedPhysicsObject.o

# target to build an object file
src/renderedPhysicsObject.cpp.o:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/renderedPhysicsObject.cpp.o
.PHONY : src/renderedPhysicsObject.cpp.o

src/renderedPhysicsObject.i: src/renderedPhysicsObject.cpp.i

.PHONY : src/renderedPhysicsObject.i

# target to preprocess a source file
src/renderedPhysicsObject.cpp.i:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/renderedPhysicsObject.cpp.i
.PHONY : src/renderedPhysicsObject.cpp.i

src/renderedPhysicsObject.s: src/renderedPhysicsObject.cpp.s

.PHONY : src/renderedPhysicsObject.s

# target to generate assembly for a file
src/renderedPhysicsObject.cpp.s:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/renderedPhysicsObject.cpp.s
.PHONY : src/renderedPhysicsObject.cpp.s

src/sprite.o: src/sprite.cpp.o

.PHONY : src/sprite.o

# target to build an object file
src/sprite.cpp.o:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/sprite.cpp.o
.PHONY : src/sprite.cpp.o

src/sprite.i: src/sprite.cpp.i

.PHONY : src/sprite.i

# target to preprocess a source file
src/sprite.cpp.i:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/sprite.cpp.i
.PHONY : src/sprite.cpp.i

src/sprite.s: src/sprite.cpp.s

.PHONY : src/sprite.s

# target to generate assembly for a file
src/sprite.cpp.s:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/sprite.cpp.s
.PHONY : src/sprite.cpp.s

src/vector2.o: src/vector2.cpp.o

.PHONY : src/vector2.o

# target to build an object file
src/vector2.cpp.o:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/vector2.cpp.o
.PHONY : src/vector2.cpp.o

src/vector2.i: src/vector2.cpp.i

.PHONY : src/vector2.i

# target to preprocess a source file
src/vector2.cpp.i:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/vector2.cpp.i
.PHONY : src/vector2.cpp.i

src/vector2.s: src/vector2.cpp.s

.PHONY : src/vector2.s

# target to generate assembly for a file
src/vector2.cpp.s:
	$(MAKE) -f CMakeFiles/Spacegame.dir/build.make CMakeFiles/Spacegame.dir/src/vector2.cpp.s
.PHONY : src/vector2.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... Spacegame"
	@echo "... src/UI.o"
	@echo "... src/UI.i"
	@echo "... src/UI.s"
	@echo "... src/camera.o"
	@echo "... src/camera.i"
	@echo "... src/camera.s"
	@echo "... src/debugUI.o"
	@echo "... src/debugUI.i"
	@echo "... src/debugUI.s"
	@echo "... src/main.o"
	@echo "... src/main.i"
	@echo "... src/main.s"
	@echo "... src/physics.o"
	@echo "... src/physics.i"
	@echo "... src/physics.s"
	@echo "... src/renderedPhysicsObject.o"
	@echo "... src/renderedPhysicsObject.i"
	@echo "... src/renderedPhysicsObject.s"
	@echo "... src/sprite.o"
	@echo "... src/sprite.i"
	@echo "... src/sprite.s"
	@echo "... src/vector2.o"
	@echo "... src/vector2.i"
	@echo "... src/vector2.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -H$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

