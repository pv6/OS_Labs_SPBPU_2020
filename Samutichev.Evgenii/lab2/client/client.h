#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED
#include "goatling.h"
#include "../core/conn.h"
#include "../core/my_semaphore.h"

class Client {
public:
    Client(size_t connectionID);
    ~Client();

    void run(Semaphore& hostSem, Semaphore& clientSem);

private:
    Connection* _conn;
    Goatling _goatling;
};

#endif // CLIENT_H_INCLUDED
