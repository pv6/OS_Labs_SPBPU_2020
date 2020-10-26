CC=g++
CFLAGS=-c -Wall -Werror

all: ex clean

ex: main.o daemon.o parser.o
	$(CC) main.o daemon.o parser.o -o daemon

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

daemon.o: daemon.cpp
	$(CC) $(CFLAGS) daemon.cpp

parser.o: parser.cpp
	$(CC) $(CFLAGS) parser.cpp

clean:
	rm -rf *.o