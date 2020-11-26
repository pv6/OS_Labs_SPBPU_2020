#ifndef WOLF_H
#define WOLF_H

#include <syslog.h>
#include <cstdlib>

#include "../interface/conn.h"
#include "../utils/SemaphoreWrapper.h"

class Wolf
{
public:
    static Wolf& getInstance(int id);
    void run(SemaphoreWrapper& sem_host, SemaphoreWrapper& sem_server);

private:
    Wolf(int id);
    Wolf(const Wolf&) = delete;
    Wolf& operator=(const Wolf&) = delete;

    bool catchGoat();

private:
    const int WOLF_MAX_SCORE = 100;
    const int ALIVE_SCORE_DIFF = 70;
    const int DEAD_SCORE_DIFF = 20;
    const int WIN_STEPS = 2;

    Conn conn;
    int cntSteps, cntDeadMove, isAlive;
};

#endif //WOLF_H
