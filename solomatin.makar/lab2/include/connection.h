#pragma once

#include <string>
#include <semaphore.h>

class Connection {
    int id, fd;
    bool hostConnection;

    Connection();
    Connection &operator=(Connection &) = delete;
    Connection(Connection &) = delete;
public:
    sem_t *clientSemaphore, *serverSemaphore;
    double lifetime = 0;

    ~Connection();

    static Connection *connect(int id);
    static Connection *create(int id);

    bool read(char *buffer, int len);
    bool write(char *buffer, int len);
};