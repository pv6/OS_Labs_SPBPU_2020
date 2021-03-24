all: build_all

define build
	g++ -Wall -Werror src/host/* src/client/* src/util/Semaphore.cpp src/util/Connection_$(1).cpp -o host_$(1) -lrt -pthread
endef

build_all:
	$(call build,mmap)
	$(call build,pipe)
	$(call build,shm)
