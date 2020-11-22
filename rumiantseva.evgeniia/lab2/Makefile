CFLAGS = -std=c++11 -Wall -Werror
PFLAGS = -pthread -lrt

CONNECTIONS = $(shell find ./conn -name '*.cpp')

all:
	@$(foreach conn,$(wildcard $(CONNECTIONS)),$(call build,$(conn:./conn/conn_%.cpp=%)))

define build
    echo $(1) ;
	g++ $(CFLAGS) ./host/* ./conn/*.h ./conn/conn_$(1).cpp -o host_$(1) $(PFLAGS) ;
	echo host_$(1) ;
	g++ $(CFLAGS) ./client/* ./conn/*.h ./conn/conn_$(1).cpp -o client_$(1) $(PFLAGS) ;
	echo client_$(1) ;
endef

clean:
	@rm -f host_* client_*
