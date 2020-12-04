#pragma once

#include <string>
#include <semaphore.h>

class Connection {
    bool hostConnection;
    std::string filename;

public:
    sem_t *clientSemaphore, *serverSemaphore;
    int id, fd, acceptFd;
    double lifetime = 0;

    Connection(int id, bool create = false);
    ~Connection();

    bool read(char *buffer, int len);
    bool write(char *buffer, int len);
    bool accept(); // only for socket
};