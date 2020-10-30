#bin/bash

$pid_file = "/var/run/lab1"

if [[ ! -f "$pid_file" ]]
then
	touch "$pid_file"
fi

chmod 666 "$pid_file"

g++ -Wall -Werror -o lab1 Demon.h Demon_impl.cpp NeededInfo.h NeededInfo_impl.cpp SubMethods.h SubMethods_impl.cpp lab1.cpp
chmod +x script_for_test.sh
./script_for_test.sh
