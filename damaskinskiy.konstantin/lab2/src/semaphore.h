#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <string>
#include <semaphore.h>

class Semaphore
{
public:
    Semaphore();
    Semaphore( std::string const& name );
    void tryOpen( std::string const& name );
    void open( std::string const& name );
    void close();
    void create( std::string const& name );
    void decrement();
    void timedDecrement();
    void increment();
    ~Semaphore();
private:
    void validate(const int rc, const std::string& caller) const;
    sem_t *sem = nullptr;
    const char *name;
    const int timeoutSec = 5;
};

#endif // SEMAPHORE_H
