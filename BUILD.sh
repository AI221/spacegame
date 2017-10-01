CC=gcc
CXX=/usr/bin/g++
if [ ! -f Makefile ]; then
	echo ";;			No Makefile found. Running CMake"
fi
#make clean
cmake .
echo ";;			Running make clean & make "
make -j5
