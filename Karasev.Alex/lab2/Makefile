all:
	g++ -Wall -Werror connect/IConnection.h connect/conn_seg.cpp connect/Connection_info.h connect/Connection_info.cpp client/client.h client/Client_impl.cpp connect/DateAndTemp.h connect/DateAndTemp.cpp host/host_impl.cpp host/Host.h host/host.cpp -lrt -pthread
	rm -rf */*.gch