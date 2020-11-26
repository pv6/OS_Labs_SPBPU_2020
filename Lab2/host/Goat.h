#ifndef GOAT_H
#define GOAT_H

#include <cstdlib>
#include <ctime>

#include "../interface/conn.h"
#include "../utils/SemaphoreWrapper.h"

class Goat
{
public:
    static Goat& getInstance(int id);
    void run(SemaphoreWrapper& sem_host, SemaphoreWrapper& sem_server);
private:
    void step();
    void acquireStatus();

private:
    const int ALIVE_MAX = 100;
    const int DEAD_MAX = 50;
    
    Conn conn;
    int isAlive;

    Goat(int id);
    Goat(const Goat&) = delete;
    Goat& operator=(const Goat&) = delete;
};

#endif //GOAT_H
