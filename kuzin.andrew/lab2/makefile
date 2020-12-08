all: build_all

define build
	g++ -std=c++17 -Wall -Werror -I ./interfaces/ ./host/* ./interfaces/* ./conn/Conn$(1).cpp -o host$(1) -pthread -lrt;
	g++ -std=c++17 -Wall -Werror -I ./interfaces/ ./client/* ./interfaces/* ./conn/Conn$(1).cpp -o client$(1) -pthread -lrt;
endef

build_all:
	$(foreach conn,$(wildcard $(shell find ./conn/ -name '*.cpp')),$(call build,$(conn:./conn/Conn%.cpp=%)))

clean:
	rm -f host* client*
