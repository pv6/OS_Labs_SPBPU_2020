all: build_all

define build
	g++ -Wall -Werror host/* client/* core/my_semaphore.cpp core/sys_exception.cpp core/conn_$(1).cpp -o host_$(1) -lrt -pthread
endef

build_all: 
	$(call build,mq)
	$(call build,sock)
