#!/bin/bash
# will be removed at reboot
sudo mkdir -p /var/run/lab2
sudo chown $USER:$USER /var/run/lab2
make
