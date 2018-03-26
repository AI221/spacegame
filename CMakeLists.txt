cmake_minimum_required (VERSION 2.6)
project (Spacegame)

set (PROJECT_NAME Spacegame)

set (Spacegame_VERSION_MAJOR 0)
set (Spacegame_VERSION_MINOR 1)

set (PROJECT_BINARY_DIR build)
set (PROJECT_SOURCE_DIR src)

set (EXECUTABLE_OUTPUT_PATH bin)

set( CMAKE_EXPORT_COMPILE_COMMANDS ON )


#option
option(WINDOWS "WINDOWS" OFF)
option(RELEASE "RELEASE" OFF) #release optimizations, slow compile no debug info


#global compilation flags
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++17 -march=haswell -Wall -Wextra -Wno-long-long -pedantic -Wswitch -Wimplicit-fallthrough ")

#nix
if (NOT WINDOWS)
	set (CMAKE_C_COMPILER "/usr/bin/gcc")
	set (CMAKE_CXX_COMPILER "/usr/bin/g++")
else ()
	#win
	set (CMAKE_SYSTEM_NAME "Windows")
	set (CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	set (CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	set (CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
	set (COMPILER_PREFIX x86_64-w64-mingw32-)
	set (CMAKE_C_COMPILER ${COMPILER_PREFIX}gcc)
	set (CMAKE_CXX_COMPILER ${COMPILER_PREFIX}c++)
	set (CMAKE_RC_COMPILER ${COMPILER_PREFIX}windres)
	set (CMAKE_PKGCONFIG_EXECUTABLE ${COMPILER_PREFIX}pkg-config)
	set (PKG_CONFIG_EXECUTABLE ${COMPILER_PREFIX}pkg-config)

	#Remove the -rdynamic option that mingw doesn't like
	set (CMAKE_SHARED_LIBRARY_LINK_C_FLAGS "")
	set (CMAKE_SHARED_LIBRARY_LINK_CXX_FLAGS "") 


	SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")


	#set (TARGET_ENVIRONMENT /usr/i686-w64-mingw32)
	set (CMAKE_FIND_ROOT_PATH ${TARGET_ENVIRONMENT})

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}-lmingw32 -mwindows -static-libgcc -static-libstdc++ -fpermissive -lws2_32")



endif ()

file(GLOB spacegame_SRC
	"src/*.h"
	"src/*.cpp"
	"src/*.c"
)



if( RELEASE)
	#append args to flags

	set(CMAKE_BUILD_TYPE Release)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -flto")
else ()
	#append args to flags
	#set(CMAKE_BUILD_TYPE Debug)

	#TODO add fsanitize for leak,address
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3 -fsanitize=undefined,thread -fstack-protector-all -Wstack-protector -fstack-check -fvtv-counts --param ssp-buffer-size=4  -O0 -fno-omit-frame-pointer -fverbose-asm -mno-omit-leaf-frame-pointer -mtune=generic -fvar-tracking -D_GLIBCXX_DEBUG=1 -frecord-gcc-switches -femit-class-debug-always -fmath-errno -fno-eliminate-unused-debug-symbols -fno-eliminate-unused-debug-types -fno-merge-debug-strings -fexceptions -fvar-tracking -fbounds-check -rdynamic -UNDEBUG -DDEBUG=1 -fno-stack-check")
endif ()





#configure_file (
#	"${PROJECT_SOURCE_DIR}/TutorialConfig.h.in"
#	"${PROJECT_BINARY_DIR}/TutorialConfig.h"
#)

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}" "include/" ) 

# add the executable
add_executable(${PROJECT_NAME} ${spacegame_SRC} "include/jsoncpp/jsoncpp.cpp" )




INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf>=2.0.0)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image>=2.0.0)


find_package (Threads)




INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} )
#NOTE THE ORDER. Threads MUST come first or you're in for hell with compiler errors
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} ${SDL2_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2IMAGE_LIBRARIES} m)  #m is the math library



