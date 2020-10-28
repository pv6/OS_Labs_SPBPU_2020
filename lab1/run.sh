#!/bin/bash

sudo touch /var/lab/lab_daemon_pid.pid
sudo chown $USER:$USER /var/lab/lab_daemon_pid.pid

g++ -Wall -Werror -o lab_daemon file_handler.h file_handler_impl.cpp daemon.h daemon_impl.cpp main.cpp
