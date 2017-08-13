cmake_minimum_required (VERSION 2.6)
project (Spacegame)

set (PROJECT_NAME Spacegame)

set (Spacegame_VERSION_MAJOR 0)
set (Spacegame_VERSION_MINOR 1)

set (PROJECT_BINARY_DIR build)
set (PROJECT_SOURCE_DIR src)

set (EXECUTABLE_OUTPUT_PATH bin)

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )

#nix
set (CMAKE_C_COMPILER "/usr/bin/gcc")
set (CMAKE_CXX_COMPILER "/usr/bin/g++")

#win
#set (CMAKE_C_COMPILER "/usr/bin/i686-w64-mingw32-gcc-posix")
#set (CMAKE_CXX_COMPILER "/usr/bin/i686-w64-mingw32-g++-posix")
#set (CMAKE_RC_COMPILER "/usr/bin/i686-w64-mingw32-windres")
#set (CMAKE_SYSTEM_NAME Windows)
#set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
#set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
#set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)


file(GLOB spacegame_SRC
	"src/*.h"
	"src/*.cpp"
	"src/*.c"
)



set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -g3 -export-dynamic")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3")




#configure_file (
#	"${PROJECT_SOURCE_DIR}/TutorialConfig.h.in"
#	"${PROJECT_BINARY_DIR}/TutorialConfig.h"
#)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

# add the executable
add_executable(${PROJECT_NAME} ${spacegame_SRC})




INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)

find_package (Threads)




INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS})
#NOTE THE ORDER. Threads MUST come first or you're in for hell with compiler errors
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2IMAGE_LIBRARIES} m) #m is the math library



