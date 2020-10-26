#/bin/bash

# compile project
mkdir build
cd build
cmake ..
make

# move executable to the root dir
mv lab1 ../lab1

# delete temporary files
cd ..
rm -r build

# create dir for the PID file
cd /var/run
sudo mkdir lab1
sudo chown nikita:nikita lab1

