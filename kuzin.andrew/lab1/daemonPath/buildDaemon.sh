#!/bin/bash

pidFilePath="/var/run/lab1.pid"

[[ -f pidFilePath ]] || sudo touch "$pidFilePath"

sudo chmod 666 "$pidFilePath"

cmake -Wall -Werror .
make
rm CMakeCache.txt Makefile cmake_install.cmake
FILE=daemon.cbp
if [[ -f "$FILE" ]]; then
    rm "$FILE"
fi
rm -r CMakeFiles/
