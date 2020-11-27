#!/bin/bash
# will be removed at reboot
sudo mkdir -p /var/run/via8_lab2
sudo chown $USER:$USER /var/run/via8_lab2
# whatever
rm /var/run/via8_lab2/* 2> /dev/null
make
