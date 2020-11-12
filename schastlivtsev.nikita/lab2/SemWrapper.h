#ifndef SEMWRAPPER_H_INCLUDED
#define SEMWRAPPER_H_INCLUDED

#include <semaphore.h>
#include <string>

class SemWrapper {
public:
    SemWrapper(const std::string& semName);
    virtual ~SemWrapper();

    void wait();
    void timedWait();
    void post();
private:
    sem_t* sem;
    static const int timeoutSec = 5;

    // functions
    void validate(const int rc, const std::string& caller) const;
};

#endif // SEMWRAPPER_H_INCLUDED
