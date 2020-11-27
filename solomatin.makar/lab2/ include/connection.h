#pragma once
#include <cstdlib>

class Connection {
private:
    int fd; // file descriptor to write and read from

    Connection(Connection &) = delete;
    Connection &operator=(Connection &) = delete;        
public:
    bool read(void *buffer, size_t count);
    bool write(void *buffer, size_t count);
    Connection(int fd);
    ~Connection();
};