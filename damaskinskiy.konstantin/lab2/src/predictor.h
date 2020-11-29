#ifndef PREDICTOR_H
#define PREDICTOR_H

#include <unistd.h>
#include <limits>
#include <signal.h>
#include "semaphore.h"
#include "conn.h"

class Predictor
{
public:
    void setHostPid( __pid_t pid );
    bool connectToHost();
    void predict();
    void terminate();

    static Predictor & get();

private:
    Predictor();

    Predictor( Predictor const& ) = delete;
    Predictor & operator=( Predictor const& ) = delete;

    static void sigHandler(int signum, siginfo_t* si, void* ucontext);

    __pid_t hostPid = std::numeric_limits<__pid_t>::max();
    Conn conn;
    Semaphore
        semPred,
        semHost;

    bool run = true;

    static Predictor instance;
};

#endif // PREDICTOR_H
