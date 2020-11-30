all:
	g++ -g -Iinclude -pthread -Wall -Werror -o server src/host/host.cpp src/host/server.cpp src/conn/conn_mq.cpp
	g++ -g -Iinclude -pthread -Wall -Werror -o client src/client/client.cpp src/conn/conn_mq.cpp src/client/forecaster.cpp
