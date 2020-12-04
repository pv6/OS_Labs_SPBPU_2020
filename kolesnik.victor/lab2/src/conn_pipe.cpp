#include "../include/Conn.h"
#include <unistd.h>


Conn::Conn(size_t id)
    : _id(id), _create(true)
{
    _fd = new int[2];
    pipe(_fd);
}
Conn::Conn(Conn &conn) 
    : _id(conn._id), _create(!conn._create)
{
    _fd = new int[2];
    _fd[0] = conn._fd[0];
    _fd[1] = conn._fd[1];
}
bool Conn::write(void *buf, size_t count) {
    int res = ::write(_fd[1], buf, count);
    if (res == -1) {
        return false;
    } else {
        return true;
    }
}
bool Conn::read(void *buf, size_t count) {
    int res = ::read(_fd[0], buf, count);
    if (res == -1) {
        return false;
    } else {
        return true;
    }
}
Conn::~Conn() {
    if (_create) {
        close(_fd[0]);
        close(_fd[1]);
    }
    delete[] _fd;
}