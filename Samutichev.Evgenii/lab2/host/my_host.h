#ifndef MY_HOST_H_INCLUDED
#define MY_HOST_H_INCLUDED
#include "../core/conn.h"
#include "wolfer.h"
#include "../core/my_semaphore.h"

class Host {
public:
    static Host* get();
    ~Host();

    void run();

private:
    static Host* _instance;
    Host();
    Host(const Host& other) = delete;
    Host& operator=(const Host& other) = delete;

    void work();

    Connection* _conn;
    Semaphore _hostSem;
    Semaphore _clientSem;
    size_t _currentTurn;
    Wolfer _wolfer;
};

#endif // MY_HOST_H_INCLUDED
