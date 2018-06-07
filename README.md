# Project moved

This repo is no longer maintained. Please see https://gitlab.com/AI221/spacegame/


# About

This is a Gamejam entry based off my 2D engine. I'm still new to C++ (and this project might require some butchering of the engine) so I'm making this a seperate branch.

# Style

The syntax for functions is pretty much the same as SDL. GE_CreateStructName , GE_FreeStructName . 
new and delete are used. 

Callbacks are C_Name

Array counts are numArrayName

Code is documented with Doxygen

# Building & running

To build:

./BUILD.sh 

To run:

./RUN.sh

You'll need:

* SDL2
* SDL2_TTF
* SDL_Image
* Unix sockets
* Pthread
