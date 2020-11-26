#include "conn_fifo.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <syslog.h>
#include <cstring>

void ConnFifo::openConn() {
    int flg = 0777;
    if (isCreator && mkfifo(name_path.c_str(), flg) == -1)
        throw std::runtime_error("fifo failed, error " + std::string(strerror(errno)));
    this->id = open(name_path.c_str(), O_RDWR);
    if (this->id == -1)
            throw std::runtime_error("fifo failed, error " + std::string(strerror(errno)));
}

void ConnFifo::readConn(char *buf, size_t count) {
    if (buf == nullptr)
	    throw std::runtime_error("nullptr buf passed into fifo reading");
    if (count != msgSize)
	    throw std::runtime_error("wrong msg size, it's should be 10: dd.mm.yyyy");
    if (read(id, buf, size) == -1) {
        throw std::runtime_error("fifo reading error " + std::string(strerror(errno)));
    }
}

void ConnFifo::writeConn(char *buf, size_t count) {
    if (buf == nullptr)
	    throw std::runtime_error("nullptr buf passed into fifo writing");
    if (count != msgSize)
	    throw std::runtime_error("wrong msg size, it's should be 10: dd.mm.yyyy");
    if (write(id, buf, size) == -1) {
        throw std::runtime_error("fifo writing error " + std::string(strerror(errno)));
    }
}

void ConnFifo::closeConn() {
    if (close(id) < 0 || (isCreator && remove(name_path.c_str()) < 0))
        throw std::runtime_error("error in closing " + std::string(strerror(errno)));
}
