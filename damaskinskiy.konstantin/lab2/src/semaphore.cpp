#include <fcntl.h>
#include <stdexcept>
#include <cstring>

#include "semaphore.h"

Semaphore::Semaphore() : sem(nullptr) {}

Semaphore::Semaphore( std::string const& name )
{
    open(name);
}

void Semaphore::open(const std::string &name)
{
    sem = sem_open(name.c_str(), O_RDWR);
    if (sem == SEM_FAILED)
        validate(-1, "sem_open: open");
}

void Semaphore::close()
{
    validate(sem_close(sem), "sem_close");
}

void Semaphore::create(const std::string &name)
{
    sem = sem_open(name.c_str(), O_CREAT & O_EXCL);
    if (sem == SEM_FAILED)
        validate(-1, "sem_open: create");
}

void Semaphore::validate(const int rc, const std::string& caller) const {
    if (rc != -1)
        return; // no error
    if (errno == ETIMEDOUT)
        throw std::runtime_error("sem timed wait: timeout expired");
    throw std::runtime_error(caller + " returned error " + strerror(errno));
}

Semaphore::~Semaphore() {
    sem_destroy(sem);
}

void Semaphore::decrement() {
    int rc;
    while ((rc = sem_wait(sem)) == EINTR);  // loop
    validate(rc, "wait");
}

void Semaphore::timedDecrement() {
    struct timespec ts; // for timeouts

    int rc = clock_gettime(CLOCK_REALTIME, &ts);

    if (rc == -1)
        throw std::runtime_error("Can't get current time for sem_timedwait");

    ts.tv_sec += timeoutSec;

    while ((rc = sem_timedwait(sem, &ts)) == EINTR);
    validate(rc, "timedWait");
}

void Semaphore::increment() {
    sem_post(sem);
}
