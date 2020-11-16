#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED
#include "goatling.h"
#include "../core/conn.h"
#include "../core/my_semaphore.h"

class Client {
public:
    Client(size_t connectionID, Semaphore hostSem, Semaphore clientSem);
    ~Client();

    void work();

private:
    Connection* _conn;
    Semaphore _hostSem;
    Semaphore _clientSem;
    Goatling _goatling;
};

#endif // CLIENT_H_INCLUDED
