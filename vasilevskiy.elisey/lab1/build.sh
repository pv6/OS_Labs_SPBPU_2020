#!/bin/bash
pidFile="/var/run/lab1.pid"
[[ -f pidFile ]] || sudo touch "$pidFile"
sudo chmod 0666 "$pidFile"
cmake -Wall -Werror .
make
rm CMakeCache.txt Makefile cmake_install.cmake
FILE=lab1.cbp
if [[ -f "$FILE" ]]; then
    rm "$FILE"
fi
rm -r CMakeFiles/
