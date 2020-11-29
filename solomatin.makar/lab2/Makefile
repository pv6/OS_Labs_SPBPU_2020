all:
	g++ -g -Iinclude -pthread -Wall -Werror -o server src/host.cpp src/server.cpp src/conn_mq.cpp
	g++ -g -Iinclude -Wall -Werror -o client src/client.cpp src/conn_mq.cpp src/forecaster.cpp
