#include "conn.h"

std::string Conn::name_path = "/tmp/lab2/fifo_6";

void Conn::openConn(bool isCreator) {
    this->isCreator = isCreator;
    //int flg = 0777;
    if (isCreator && mkfifo(Conn::name_path.c_str(), 0777) == -1)
        throw std::runtime_error("fifo failed, error " + std::string(strerror(errno)));
    this->id = open(Conn::name_path.c_str(), O_RDWR);
    if (this->id == -1)
            throw std::runtime_error("fifo failed, error " + std::string(strerror(errno)));
}

void Conn::readConn(char *buf, size_t count) {
    if (buf == nullptr)
	    throw std::runtime_error("nullptr buf passed into fifo reading");
    if (count > msgSize)
	    throw std::runtime_error("wrong msg size, it's should be 10: dd.mm.yyyy");
    if (read(this->id, buf, count) == -1) {
        throw std::runtime_error("fifo reading error " + std::string(strerror(errno)));
    }
}

void Conn::writeConn(char *buf, size_t count) {
    if (buf == nullptr)
	    throw std::runtime_error("nullptr buf passed into fifo writing");
    if (count > msgSize)
	    throw std::runtime_error("wrong msg size, it's should be 10: dd.mm.yyyy");
    if (write(this->id, buf, count) == -1) {
        throw std::runtime_error("fifo writing error " + std::string(strerror(errno)));
    }
}

void Conn::closeConn() {
    std::cout << "id in open " << this->id << std::endl; 
    if (close(this->id) < 0 || (isCreator && remove(Conn::name_path.c_str()) < 0))
        throw std::runtime_error("error in closing " + std::string(strerror(errno)));
}
