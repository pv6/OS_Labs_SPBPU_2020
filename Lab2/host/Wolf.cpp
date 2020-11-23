#include <iostream>
#include <cstdlib>

#include "Wolf.h"

Wolf::Wolf(int id)
    : conn(id, true), cntSteps(1), cntDeadMove(0), isAlive(1)
{
    srand(time(nullptr));
}

Wolf& Wolf::getInstance(int id)
{
    static Wolf instance(id);
    return instance;
}

void Wolf::run(SemaphoreWrapper& sem_host, SemaphoreWrapper& sem_server)
{
    syslog(LOG_INFO, "Game is started");

    bool isGameOver = false;

    while (!isGameOver) {
        syslog(LOG_INFO, "Wolf run time ");

        sem_server.post();
        sem_host.timedWait();

        isGameOver = catchGoat();

        sem_server.post();
    }
}

bool Wolf::catchGoat()
{
    printf("Step: %d\n", cntSteps);

    int goatNum = conn.Read();

    if (isAlive)
        printf("Alive goat score: %d\n", goatNum);
    else
        printf("Dead goat score: %d\n", goatNum);

    int wolfNum = rand() % WOLF_MAX_SCORE + 1;

    printf("Wolf score: %d\n", wolfNum);

    int diff = abs(wolfNum - goatNum);

    if (isAlive)
    {
        if (diff <= ALIVE_SCORE_DIFF)
            printf("Diff = |%d - %d| = %d <= %d => Alive\n", wolfNum, goatNum, diff, ALIVE_SCORE_DIFF);
        else
        {
            isAlive = 0;
            printf("Diff = |%d - %d| = %d  > %d => Dead\n", wolfNum, goatNum, diff, ALIVE_SCORE_DIFF);
        }
    }
    else
    {
        if (diff <= DEAD_SCORE_DIFF)
        {
            printf("Diff = |%d - %d| = %d <= %d => Alive\n", wolfNum, goatNum, diff, DEAD_SCORE_DIFF);
            isAlive = 1;
            cntDeadMove = 0;
        }
        else
        {
            printf("Diff = |%d - %d| = %d > %d => Dead\n", wolfNum, goatNum, diff, DEAD_SCORE_DIFF);
            cntDeadMove++;

            if (cntDeadMove == 2)
            {
                printf("2 dead goat's moves in a row => Game over\n");

                return true;
            }
        }
    }

    conn.Write(isAlive);

    cntSteps++;

    return false;
}
