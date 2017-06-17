if [ ! -f Makefile ]; then
	echo ";;			No Makefile found. Running CMake"
	cmake .
fi
echo ";;			Running make clean & make "
make clean
make
