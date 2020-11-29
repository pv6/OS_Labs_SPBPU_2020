#include <fcntl.h>
#include <stdexcept>
#include <cstring>
#include <syslog.h>

#include "semaphore.h"

Semaphore::Semaphore() : sem(nullptr) {}

Semaphore::Semaphore( std::string const& name ) :
    name(name.c_str())
{
    open(name);
}

void Semaphore::tryOpen(const std::string &name)
{
    try
    {
        open(name);
        syslog(LOG_INFO, "Semaphore %s opened!", name.c_str());
    }
    catch (...)
    {
        syslog(LOG_INFO, "Couldn't open semaphore. Create it!");
        create(name);
        syslog(LOG_INFO, "Semaphore %s created!", name.c_str());
    }
}

void Semaphore::open(const std::string &name)
{
    sem = sem_open(("/" + name).c_str(), O_RDWR);
    if (sem == SEM_FAILED)
        validate(-1, "sem_open: open");
}

void Semaphore::close()
{
    if (sem != nullptr)
        validate(sem_close(sem), "sem_close");
    sem = nullptr;
}

void Semaphore::create(const std::string &name)
{
    sem = sem_open(("/" + name).c_str(), O_CREAT | O_EXCL, 0666, 0);
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
}

void Semaphore::decrement() {
    int rc = sem_wait(sem);
//    int rc;
//    while ((rc = sem_wait(sem)) == EINTR);  // loop
    validate(rc, "wait");
}

void Semaphore::timedDecrement() {
    struct timespec ts; // for timeouts

    int rc = clock_gettime(CLOCK_REALTIME, &ts);

    if (rc == -1)
        throw std::runtime_error("Can't get current time for sem_timedwait");

    ts.tv_sec += timeoutSec;

    //while ((rc = sem_timedwait(sem, &ts)) == EINTR);
    rc = sem_timedwait(sem, &ts);
    validate(rc, "timedWait");
}

//void Semaphore::unlink()
//{
//    if (sem != nullptr)
//        sem_unlink(name);
//}

void Semaphore::increment() {
    sem_post(sem);
}
