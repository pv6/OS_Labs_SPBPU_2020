#! /bin/bash

pidFile="/var/run/lab1.pid"

[[ -f pidFile ]] || sudo touch "$pidFile"

sudo chmod 0666 "$pidFile"

make
