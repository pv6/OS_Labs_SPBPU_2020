#include "ProcedureBoss.h"
#include <unistd.h> // for sleep

ProcedureBoss::ProcedureBoss(const size_t waitInterval) : waitSec(waitInterval) {}

ProcedureBoss::~ProcedureBoss()
{
    //dtor
}


void ProcedureBoss::launchProcCycle(DaemonProcedure& runnable) {
    while (notStopped) {
        runnable.run(); // daemon works
        sleep(waitSec); // daemon waits
    }
}

void ProcedureBoss::finishWork() {
    notStopped = false; // should be stopped now
}

void ProcedureBoss::setWaitSec(size_t const waitInterval) {
    waitSec = waitInterval;
}
