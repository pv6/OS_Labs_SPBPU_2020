#ifndef MY_SEMAPHORE_H_INCLUDED
#define MY_SEMAPHORE_H_INCLUDED
#include <semaphore.h>
#include <string>
#include <time.h>

class Semaphore {
public:
    Semaphore(const char* name);
    ~Semaphore();

    void wait();
    bool timedWait(time_t timeout);
    void post();

private:
    Semaphore(const Semaphore& other) = delete;

    std::string _name;
    sem_t* _sem;
};

#endif // MY_SEMAPHORE_H_INCLUDED
