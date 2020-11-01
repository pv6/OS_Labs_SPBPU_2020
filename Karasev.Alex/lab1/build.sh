#!/bin/bash

pidFilePath="/var/run/daemon_pid.pid"

[[ -f pidFilePath ]] || sudo touch "$pidFilePath"

sudo chmod 666 "$pidFilePath"

cmake -Wall -Werror .
make
rm CMakeCache.txt Makefile cmake_install.cmake
file=Lab1.cbp
if [[ -f "$file" ]]; then
	rm "$file"
fi
rm -r CMakeFiles/
