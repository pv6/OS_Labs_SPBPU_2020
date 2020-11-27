CC = g++
CFLAGS = -Wall -Werror -std=c++11 -o
LFLAGS = -lpthread -lrt
SRCDIR = src
BLDDIR = build

SOURCES_COMMON = $(addprefix $(SRCDIR)/, $(addsuffix .cpp, player custom_exception))

SOURCES_HOST   = $(addprefix $(SRCDIR)/, $(addsuffix .cpp, player_wolf host))
SOURCES_CLIENT = $(addprefix $(SRCDIR)/, $(addsuffix .cpp, player_goat client))

SOURCES_FIFO   = $(addprefix $(SRCDIR)/, $(addsuffix .cpp, conn_fifo))
SOURCES_SEG    = $(addprefix $(SRCDIR)/, $(addsuffix .cpp, conn_seg))
SOURCES_SHM    = $(addprefix $(SRCDIR)/, $(addsuffix .cpp, conn_shm))

all: dir host_fifo host_seg host_shm client_fifo client_seg client_shm clear

dir:
	mkdir -p $(BLDDIR)

host_fifo: $(SOURCES_COMMON) $(SOURCES_HOST) $(SOURCES_FIFO)
	$(CC) $(CFLAGS) $(BLDDIR)/$@ $^ $(LFLAGS)

host_seg: $(SOURCES_COMMON) $(SOURCES_HOST) $(SOURCES_SEG)
	$(CC) $(CFLAGS) $(BLDDIR)/$@ $^ $(LFLAGS)

host_shm: $(SOURCES_COMMON) $(SOURCES_HOST) $(SOURCES_SHM)
	$(CC) $(CFLAGS) $(BLDDIR)/$@ $^ $(LFLAGS)	

client_fifo: $(SOURCES_COMMON) $(SOURCES_CLIENT) $(SOURCES_FIFO)
	$(CC) $(CFLAGS) $(BLDDIR)/$@ $^ $(LFLAGS)

client_seg: $(SOURCES_COMMON) $(SOURCES_CLIENT) $(SOURCES_SEG)
	$(CC) $(CFLAGS) $(BLDDIR)/$@ $^ $(LFLAGS)

client_shm: $(SOURCES_COMMON) $(SOURCES_CLIENT) $(SOURCES_SHM)
	$(CC) $(CFLAGS) $(BLDDIR)/$@ $^ $(LFLAGS)

clear:
	rm -rf *.o 2>/dev/null
