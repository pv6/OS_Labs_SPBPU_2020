CFLAGS = -std=c++11 -Wall -Werror
PFLAGS = -pthread -lrt

CONNECTIONS = $(shell find src -name "con_*.cpp")

all:
	@$(foreach conn,$(wildcard $(CONNECTIONS)),$(call build,$(conn:src/con_%.cpp=%)))

define build
    echo $(1) ;
	g++ $(CFLAGS) src/host.cpp src/semaphore.cpp src/con_$(1).cpp -o host_$(1) $(PFLAGS) ;
	echo host_$(1) ;
	g++ $(CFLAGS) src/client.cpp src/semaphore.cpp src/predictor.cpp src/con_$(1).cpp -o client_$(1) $(PFLAGS) ;
	echo client_$(1) ;
endef

clean:
	@rm -f host_* client_*
