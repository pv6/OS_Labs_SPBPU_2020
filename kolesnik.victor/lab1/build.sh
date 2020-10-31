#!/bin/bash

cmake -Wall -Werror .
make

rm CMakeCache.txt cmake_install.cmake Makefile
rm -rf CMakeFiles

sudo touch /var/run/lab1.pid
sudo chmod 666 /var/run/lab1.pid