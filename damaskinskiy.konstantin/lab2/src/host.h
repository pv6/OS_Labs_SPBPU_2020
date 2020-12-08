#pragma once

#include <memory>
#include <vector>
#include <set>

#include <pthread.h>
#include <signal.h>

#include "semaphore.h"
#include "conn.h"

class Host
{
public:
    void setupPredictorCount( size_t count );
    static Host & get();
    void prepareSemaphore();
    void run( std::string const& date );
    void terminate();
    ~Host();
private:
    struct ThreadData
    {
        std::string date;
        __pid_t predictorPid;
    };

    Host();
    static void * requestForecast( void * );
    static void sigHandler( int signum, siginfo_t* info, void* ptr );

    Host & operator=( Host const & ) = delete;
    Host( Host &  ) = delete;
    Host( Host && ) = delete;

//    Semaphore semHost;
//    Semaphore semPred;

    std::vector<pthread_t> predictorThr;
    std::set<__pid_t> predictorPid;

    size_t estPredCount;
    size_t connPredCount = 0;

    static Host instance;
};
