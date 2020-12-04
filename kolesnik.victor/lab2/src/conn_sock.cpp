#include "../include/Conn.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/un.h>
#include <iostream>

#define FILE_PATH "/tmp/lab2_socket_"


Conn::Conn(size_t id)
    : _id(id), _create(true)
{
    _fd = new int[2];

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, (FILE_PATH + std::to_string(_id)).c_str());

    _fd[0] = socket(AF_UNIX, SOCK_STREAM, 0);
    bind(_fd[0], (const struct sockaddr *)&addr, sizeof(addr));

    listen(_fd[0], 1);

    _fd[1] = accept(_fd[0], NULL, NULL);
}
Conn::Conn(Conn &conn)
    : _id(conn._id), _create(!conn._create)
{
    _fd = new int[2];

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, (FILE_PATH + std::to_string(_id)).c_str());

    _fd[1] = socket(AF_UNIX, SOCK_STREAM, 0);
    connect(_fd[1], (const struct sockaddr *)&addr, sizeof(addr));
}
bool Conn::write(void *buf, size_t count) {
    int res = send(_fd[1], buf, count, 0);
    if (res == -1) {
        return false;
    } else {
        return true;
    }
}
bool Conn::read(void *buf, size_t count) {
    int res = recv(_fd[1], buf, count, 0);
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
        unlink((FILE_PATH + std::to_string(_id)).c_str());
    } else {
        close(_fd[1]);
    }
    delete[] _fd;
}