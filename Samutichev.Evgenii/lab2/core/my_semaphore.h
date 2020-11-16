#ifndef MY_SEMAPHORE_H_INCLUDED
#define MY_SEMAPHORE_H_INCLUDED
#include <semaphore.h>
#include <string>
#include <time.h>

class Semaphore {
public:
    Semaphore(const char* name);
    Semaphore(const Semaphore& other);
    ~Semaphore();

    void wait();
    bool timedWait(time_t timeout);
    void post();

private:
    std::string _name;
    sem_t* _sem;
    bool _creator;
};

#endif // MY_SEMAPHORE_H_INCLUDED
