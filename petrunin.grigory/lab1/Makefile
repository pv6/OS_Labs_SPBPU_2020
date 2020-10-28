CC=g++
CFLAGS=-Wall -Werror -std=c++11 -c
SRCDIR=./src
BLDDIR=./build
SOURCES=$(wildcard $(SRCDIR)/*.cpp)
OBJECTS=$(patsubst $(SRCDIR)/%.cpp, %.o, $(SOURCES))

all: dir exec clear

dir:
	mkdir -p $(BLDDIR)

exec: $(OBJECTS)
	$(CC) -o $(BLDDIR)/lab1 $^

%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@

clear:
	rm -rf *.o 2>/dev/null
