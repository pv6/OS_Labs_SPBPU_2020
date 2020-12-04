all: compile
	for conn_module in pipe mq socket; do \
	    g++ -g -Iinclude -pthread -Wall -o server_$$conn_module host.o server.o conn_$$conn_module.o -lrt ; \
	    g++ -g -Iinclude -pthread -Wall -o client_$$conn_module client.o forecaster.o conn_$$conn_module.o -lrt ; \
	done

compile:
	g++ -g -Iinclude -pthread -Wall -c src/**/*.cpp

clean:
	rm -rf *.o
