all: build_all

define build
	g++ -Wall -Werror host/* interface/* utils/* conns/conn_$(1).cpp -o host_$(1) -lrt -pthread
endef

build_all: 
	$(call build,seg)
	$(call build,mq)
	$(call build,fifo)
clean:
	rm -f host_*

