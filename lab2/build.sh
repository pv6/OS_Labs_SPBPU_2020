#!/bin/bash

g++ -Wall -Werror -o host_fifo host_main.cpp host_cls.h conn.h DTStor.h ConnHelper.h Host_Conn_Inf.h host_cls.cpp DTStor.cpp ConnHelper.cpp conn_fifo.cpp -lpthread -lrt

g++ -Wall -Werror -o client_fifo client_main.cpp client_cls.h conn.h DTStor.h ConnHelper.h Host_Conn_Inf.h client_cls.cpp DTStor.cpp ConnHelper.cpp conn_fifo.cpp -lpthread -lrt

g++ -Wall -Werror -o host_seg host_main.cpp host_cls.h conn.h DTStor.h ConnHelper.h Host_Conn_Inf.h host_cls.cpp DTStor.cpp ConnHelper.cpp conn_seg.cpp -lpthread -lrt

g++ -Wall -Werror -o client_seg client_main.cpp client_cls.h conn.h DTStor.h ConnHelper.h Host_Conn_Inf.h client_cls.cpp DTStor.cpp ConnHelper.cpp conn_seg.cpp -lpthread -lrt

g++ -Wall -Werror -o host_shm host_main.cpp host_cls.h conn.h DTStor.h ConnHelper.h Host_Conn_Inf.h host_cls.cpp DTStor.cpp ConnHelper.cpp conn_shm.cpp -lpthread -lrt

g++ -Wall -Werror -o client_shm client_main.cpp client_cls.h conn.h DTStor.h ConnHelper.h Host_Conn_Inf.h client_cls.cpp DTStor.cpp ConnHelper.cpp conn_shm.cpp -lpthread -lrt
