#!/bin/bash
make
sudo touch /var/run/lab1.pid
sudo chown $USER:$USER /var/run/lab1.pid