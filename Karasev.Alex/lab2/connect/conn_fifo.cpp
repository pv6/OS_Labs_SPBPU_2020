#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdexcept>
#include "IConnection.h"

void IConnection::openConnection(size_t id, bool create) {
    owner = create;
    name = "/tmp/lab2_pipe";
    int flg = 0777;
    if (owner && mkfifo(name.c_str(), flg) == -1) {
        throw std::runtime_error("mkfifo failed, error " + std::string(strerror(errno)));
    } 
    this->id = open(name.c_str(), O_RDWR);
    if (this->id == -1) {
            throw std::runtime_error("mkfifo failed, error " + std::string(strerror(errno)));
    }
}

void IConnection::readConnection(DateAndTemp *buf, size_t size) const {
    if (read(id, (char*)(buf), size) == -1) {
        throw std::runtime_error("reading error " + std::string(strerror(errno)));
    }
}

void IConnection::writeConnection(DateAndTemp *buf, size_t size) const {
    if (write(id, (char*)(buf), size) == -1) {
        throw std::runtime_error("writing error " + std::string(strerror(errno)));
    }
}

void IConnection::closeConnection() const {
    if (close(id) < 0 || (owner && remove(name.c_str()) < 0)) {
        throw std::runtime_error("close error " + std::string(strerror(errno)));
    }
}