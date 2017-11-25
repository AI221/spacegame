#/bin/sh
cd bin
valgrind --tool=memcheck --leak-check=full --track-origins=yes ./Spacegame
