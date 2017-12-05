#/bin/sh
cd bin
valgrind --tool=callgrind --dump-instr=yes --collect-jumps=yes ./Spacegame
