#ifndef LAB2_SEMAPHORE_HPP
#define LAB2_SEMAPHORE_HPP

#include <semaphore.h>

class Semaphore
{
public:
    Semaphore();
    ~Semaphore();

    void post();
    bool timedWait(int secondsToWait);
    void wait();
private:
    sem_t* semaphore;
};

#endif //LAB2_SEMAPHORE_HPP
