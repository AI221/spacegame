#!/bin/bash

while true; do
	echo "Press enter to check CPP"
	read Unused
	cppcheck --project=compile_commands.json --std=c++11 --inconclusive --enable=warning,performance,information,portability,style
	echo "Done."


done
