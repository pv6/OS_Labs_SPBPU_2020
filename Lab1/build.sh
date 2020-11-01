#!/bin/bash

pid_location="/var/run/daemon.pid"

if [[ ! -f "$pid_location" ]]
then
  sudo touch "$pid_location"
fi

chmod 666 "$pid_location"

g++ -Wall -Werror -o daemon -lstdc++ -std=c++11 Event.h Event.cpp daemon.h daemon.cpp main.cpp
