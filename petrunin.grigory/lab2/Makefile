CC = g++
CFLAGS = -Wall -Werror -std=c++11 -c
SRCDIR = src
BLDDIR = build
SOURCES_HOST = $(addprefix $(SRCDIR)/, host.cpp custom_exception.cpp player.cpp player_wolf.cpp)
OBJECTS_HOST = $(patsubst $(SRCDIR)/%.cpp, %.o, $(SOURCES_HOST))
SOURCES_CLIENT = $(addprefix $(SRCDIR)/, client.cpp custom_exception.cpp player.cpp player_goat.cpp)
OBJECTS_CLIENT = $(patsubst $(SRCDIR)/%.cpp, %.o, $(SOURCES_CLIENT))

all: dir host client clear

dir:
	mkdir -p $(BLDDIR)

host: $(OBJECTS_HOST)
	$(CC) -o $(BLDDIR)/host $^

client: $(OBJECTS_CLIENT)
	$(CC) -o $(BLDDIR)/client $^

%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@

clear:
	rm -rf *.o 2>/dev/null
