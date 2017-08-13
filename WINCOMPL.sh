#!/bin/sh

rm src/*.gch

i686-w64-mingw32-g++-posix  src/* -ggdb -g3 -w -I/usr/i686-w64-mingw32/include/SDL2 -lmingw32 -lSDL2main -lSDL2 -mwindows -lSDL2_ttf -lSDL2_image --std=c++11 -lm -Wl,-subsystem,console -o winport.exe

#Note, you'll need to include sdl + ttf + image , the     SDL2_image-devel-2.0.1-mingw.tar.gz -like things and move them into their proper place @ /usr/i686-w64-mingw32/ . 
#You'll need the runtimes on windows.
