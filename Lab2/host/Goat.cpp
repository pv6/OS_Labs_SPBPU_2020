#include <cstdlib>
#include <ctime>

#include "Goat.h"

Goat::Goat(int id)
    : conn(id, false), isAlive(1)
{
    srand(time(nullptr));
}

Goat& Goat::getInstance(int id)
{
    static Goat instance(id);
    return instance;
}

void Goat::run(SemaphoreWrapper &sem_host, SemaphoreWrapper &sem_server)
{
    while (true) {
        sem_server.wait();
        step();
        sem_host.post();
        sem_server.wait();
        acquireStatus();
    }
}

void Goat::step()
{
    int goatNum;

    if (isAlive)
        goatNum = rand() % ALIVE_MAX + 1;
    else
        goatNum = rand() % DEAD_MAX + 1;

    conn.Write(goatNum);
}

void Goat::acquireStatus()
{
    isAlive = conn.Read();
}