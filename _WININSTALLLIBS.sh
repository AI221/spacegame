#!/bin/bash

cd /tmp

mkdir SDLTMP 

cd SDLTMP

wget https://www.libsdl.org/release/SDL2-devel-2.0.6-mingw.tar.gz

tar -xvf SDL2-devel*

cd SDL2-*/

cd x86_64-w64-mingw32/

sudo cp ./* /usr/x86_64-w64-mingw32/ -rf --verbose

cd /tmp

mkdir IMGTMP

cd IMGTMP




wget https://www.libsdl.org/projects/SDL_image/release/SDL2_image-devel-2.0.1-mingw.tar.gz

tar -xvf SDL2_image-devel-*

cd SDL2_image-*/

cd x86_64-w64-mingw32/

sudo cp ./* /usr/x86_64-w64-mingw32/ -rf --verbose

cd /tmp

mkdir TTFTMP
cd TTFTMP/

wget https://www.libsdl.org/projects/SDL_ttf/release/SDL2_ttf-devel-2.0.14-mingw.tar.gz

tar -xvf SDL2_ttf-devel-*

cd SDL2_ttf-*/

cd x86_64-w64-mingw32/

sudo cp ./* /usr/x86_64-w64-mingw32/ -rf --verbose


echo Done.

