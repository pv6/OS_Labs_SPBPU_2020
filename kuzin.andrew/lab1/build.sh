#!/bin/bash

cmake -Wall -Werror .
make
rm CMakeCache.txt Makefile cmake_install.cmake
FILE=manager.cbp
if [[ -f "$FILE" ]]; then
    rm "$FILE"
fi
rm -r CMakeFiles/

d=$(dirname $0)
cd ${d}/daemonPath
sh buildDaemon.sh
mv daemon ../daemon
