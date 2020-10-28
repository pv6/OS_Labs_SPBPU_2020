#ifndef PROCEDUREBOSS_H
#define PROCEDUREBOSS_H

#include "DaemonProcedure.h"
#include <cstddef>

class ProcedureBoss
{
    public:
        ProcedureBoss(const std::size_t waitInterval = 0);
        virtual ~ProcedureBoss();

        void launchProcCycle(DaemonProcedure& runnable); // main cycle
        void finishWork();
        void setWaitSec(size_t const waitInterval);
    protected:

    private:
        size_t waitSec;
        bool notStopped = true;
};

#endif // PROCEDUREBOSS_H
