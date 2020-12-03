#ifndef CONN_H_INCLUDED__
#define CONN_H_INCLUDED__

#include <stddef.h>

class Conn {
public:
    Conn(size_t id);
    Conn(Conn &conn);
    bool read(void *buf, size_t count);
    bool write(void *buf, size_t count);
    ~Conn();

private:
    int *_fd;
    size_t _id;
    bool _create;
};

#endif