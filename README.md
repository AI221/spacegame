# About

This a 2D engine. It is designed around space games, and will eventually be used in a space game of my own.

# Style

Originally, I was going to program this as Object-Oriented. I then realized that was a mistake, and converted everything to C-Styled Struct-based programming.
I want the engine to be able to be used with C if possible. 
The syntax for functions is pretty much the same as SDL. GE_CreateStructName , GE_FreeStructName . 
new and delete are used. This won't be a problem for C users because FreeStructName will deletes them for you.

# Building & running

To build:

./BUILD.sh 

To run:

./RUN.sh

You'll need:

* SDL2
* SDL2_TTF
