#include "../include/Conn.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

#include <iostream>

#define FILE_PATH "/tmp/lab2_fifo_"


Conn::Conn(size_t id)
    : _id(id), _create(true)
{
    _fd = new int;
    mkfifo((FILE_PATH + std::to_string(_id)).c_str(), 0666);
    *_fd = ::open((FILE_PATH + std::to_string(_id)).c_str(), O_RDWR);
}
Conn::Conn(Conn &conn) 
    : _id(conn._id), _create(!conn._create)
{
    _fd = new int;
    *_fd = ::open((FILE_PATH + std::to_string(_id)).c_str(), O_RDWR);
}
bool Conn::write(void *buf, size_t count) {
    int res = ::write(*_fd, buf, count);
    if (res == -1) {
        return false;
    } else {
        return true;
    }
}
bool Conn::read(void *buf, size_t count) {
    int res = ::read(*_fd, buf, count);
    if (res == -1) {
        return false;
    } else {
        return true;
    }
}
Conn::~Conn() {
    close(*_fd);
    if (_create) {
        remove((FILE_PATH + std::to_string(_id)).c_str());
    }
    delete _fd;
}