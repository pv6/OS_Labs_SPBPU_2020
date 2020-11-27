CFLAGS = -std=c++11 -Wall -Werror -I ./models/
PFLAGS = -lpthread -lrt

CONNECTIONS = $(shell find ./conn/ -name '*.cpp')

all:
	@$(foreach conn,$(wildcard $(CONNECTIONS)),$(call build,$(conn:./conn/conn_%.cpp=%)))

define build
	g++ $(CFLAGS) ./host/* 	 ./models/* ./conn/conn_$(1).cpp -o host_$(1) $(PFLAGS) ;
	echo host_$(1) ;
	g++ $(CFLAGS) ./client/* ./models/* ./conn/conn_$(1).cpp -o client_$(1) $(PFLAGS) ;
	echo client_$(1) ;
endef
	
clean:
	@rm -f host_* client_*
