CC=g++

CFLAGS=-c -Wall -Werror

all: exe clear

exe: main.o daemon.o parser.o
	$(CC) main.o daemon.o parser.o -o lab1

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

daemon.o: daemon.cpp
	$(CC) $(CFLAGS) daemon.cpp

parser.o: parser.cpp
	$(CC) $(CFLAGS) parser.cpp

clear:
	rm -rf *.o
