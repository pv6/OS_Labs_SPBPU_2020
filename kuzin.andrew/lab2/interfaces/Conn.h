#ifndef LAB2_CONN_H
#define LAB2_CONN_H

#include <stdlib.h>
#include <iostream>

#define SEMAPHORE_HOST_NAME "LAB2_HOST"
#define SEMAPHORE_CLIENT_NAME "LAB2_CLIENT"


class Conn
{
public:
    Conn();
    ~Conn();

    bool connOpen(size_t id, bool create);
    bool connClose();

    bool connReceive(void *buf, size_t count);
    bool connSend(void *buf, size_t count);

private:
    bool own_;
    int id_;
    int* desc_;
};

#endif //LAB2_CONN_H
