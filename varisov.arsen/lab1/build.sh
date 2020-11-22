PID_FILE="/var/run/dir_cleaner.pid"
if [ ! -f $PID_FILE ]; then
	sudo touch $PID_FILE
	sudo chown $USER:$USER $PID_FILE
fi

cmake -B build -Wall -Werror
cd build && make

rm -rf !("DirectoryCleaner")

cd ..
