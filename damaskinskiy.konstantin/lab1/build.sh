#PID file
PID_FILE="/var/run/dm_kd.pid"
if [ ! -f $PID_FILE ]; then
	sudo touch $PID_FILE
	sudo chmod +rw $PID_FILE
	echo PID file created
fi

#make
cmake -B build -DCMAKE_BUILD_TYPE=Release
cd build && make
cd ..

#clear
rm -rf `find build ! -name "DiskMonitor" ! -name "*.conf" ! -name build`

#set LOCAL0
LOCAL0DIR="local0.* /var/log/dm_kd"
SYSLOG="/etc/rsyslog.conf"

if [[ -z `cat "$SYSLOG" | grep "$LOCAL0DIR"` ]]; then
	sudo sh -c "echo '$LOCAL0DIR' >> '$SYSLOG'"
	sudo service rsyslog restart
	echo rsyslog restart
fi

cd ..
