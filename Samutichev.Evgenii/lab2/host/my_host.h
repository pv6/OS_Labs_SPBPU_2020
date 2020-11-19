#ifndef MY_HOST_H_INCLUDED
#define MY_HOST_H_INCLUDED
#include "../core/conn.h"
#include "wolfer.h"
#include "../core/my_semaphore.h"

class Host {
public:
    Host(size_t connectionID);
    ~Host();

    void run(Semaphore& hostSem, Semaphore& clientSem);

private:
    Host(const Host& other) = delete;
    Host& operator=(const Host& other) = delete;

    void printInfo();

    Connection* _conn;
    size_t _currentTurn;
    Wolfer _wolfer;
};

#endif // MY_HOST_H_INCLUDED
