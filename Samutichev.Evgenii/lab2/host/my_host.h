#ifndef MY_HOST_H_INCLUDED
#define MY_HOST_H_INCLUDED
#include "../core/conn.h"
#include <semaphore.h>

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
    sem_t* _hostSem;
    sem_t* _clientSem;
    size_t _currentTurn;
};

#endif // MY_HOST_H_INCLUDED
