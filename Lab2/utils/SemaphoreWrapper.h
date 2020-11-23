#ifndef SEMAPHOREWRAPPER_H
#define SEMAPHOREWRAPPER_H

#include <string>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <semaphore.h>

#include <cstdlib>

class SemaphoreWrapper {
public:
    SemaphoreWrapper(const std::string &shm_name);
    void post();
    void timedWait();
    void wait();

    ~SemaphoreWrapper();
private:
    sem_t* initSem(int shm);
    int openShm();

private:
    static const int TIMEOUT = 5;
    sem_t* sem;
    std::string shm_name;
};


#endif //SEMAPHOREWRAPPER_H
