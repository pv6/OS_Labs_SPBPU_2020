#/bin/bash

# compile project
mkdir build
cd build

# mq
g++ -Wall -o host_mq ../host.cpp ../Client.h ../Client.cpp ../Conn.h ../conn_mq.cpp ../conn_mq.h ../DateHolder.cpp ../DateHolder.h ../HostWorker.cpp ../HostWorker.h ../TemperaturePredictor.cpp ../TemperaturePredictor.h ../SemWrapper.h ../SemWrapper.cpp -lpthread -lrt

# fifo
g++ -Wall -o host_fifo ../host.cpp ../Client.h ../Client.cpp ../Conn.h ../conn_fifo.cpp ../conn_fifo.h ../DateHolder.cpp ../DateHolder.h ../HostWorker.cpp ../HostWorker.h ../TemperaturePredictor.cpp ../TemperaturePredictor.h ../SemWrapper.h ../SemWrapper.cpp -lpthread -lrt

# sock
g++ -Wall -o host_sock ../host.cpp ../Client.h ../Client.cpp ../Conn.h ../conn_sock.cpp ../conn_sock.h ../DateHolder.cpp ../DateHolder.h ../HostWorker.cpp ../HostWorker.h ../TemperaturePredictor.cpp ../TemperaturePredictor.h ../SemWrapper.h ../SemWrapper.cpp -lpthread -lrt


# move executable to the sources dir
cd ..
mv build/host_mq host_mq
mv build/host_fifo host_fifo
mv build/host_sock host_sock

# remove build dir
rm -r build
