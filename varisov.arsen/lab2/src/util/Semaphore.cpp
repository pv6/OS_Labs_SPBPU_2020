#include "Semaphore.hpp"

#include <cerrno>
#include <cstring>
#include <string>
#include <syslog.h>
#include <system_error>
#include <sys/mman.h>
#include <time.h>

namespace
{
constexpr bool   semIsInterProcess     = true;
constexpr int    initialSemVal         = 0;    //Semaphore is blocked initially
constexpr size_t mmapOffset            = 0;
constexpr int    mmapFd                = -1;   //Semaphore will be located in parent process' memory
}

Semaphore::Semaphore()
{
    semaphore = static_cast<sem_t*>(mmap(nullptr,
                                         sizeof(*semaphore),
                                         PROT_READ | PROT_WRITE,
                                         MAP_ANONYMOUS | MAP_SHARED,
                                         mmapFd,
                                         mmapOffset));
    if (semaphore == MAP_FAILED)
    {
        syslog(LOG_ERR, "Could not create an anonymous mapping for semaphore. Reason: %s", strerror(errno));
        throw std::system_error();
    }
    if (sem_init(semaphore, semIsInterProcess, initialSemVal) == -1)
    {
        syslog(LOG_ERR, "Could not initialize semaphore. Reason: %s", strerror(errno));
        munmap(semaphore, sizeof(*semaphore));
        throw std::system_error();
    }
    syslog(LOG_INFO, "Semaphore created successfully");
}

Semaphore::~Semaphore()
{
    sem_destroy(semaphore);
    munmap(semaphore, sizeof(*semaphore));
}

void Semaphore::post()
{
    if (sem_post(semaphore) == -1)
    {
        syslog(LOG_ERR, "Sem_post failed. Reason: %s", strerror(errno));
        throw std::system_error();
    }
}

void Semaphore::wait()
{
    if (sem_wait(semaphore) == -1)
    {
        syslog(LOG_ERR, "Sem_wait failed. Reason: %s", strerror(errno));
        throw std::system_error();
    }
}

bool Semaphore::timedWait(int secondsToWait)
{
    struct timespec timeSpec;
    timespec_get(&timeSpec, TIMER_ABSTIME);
    timeSpec.tv_sec += secondsToWait;

    if (sem_timedwait(semaphore, &timeSpec) == -1)
    {
        if (errno == ETIMEDOUT)
        {
            syslog(LOG_INFO, "Semaphore wait timed out. Specified seconds: %d", secondsToWait);
            return false;
        }
        else
        {
            syslog(LOG_ERR, "sem_timedwait failed. Reason: %s", strerror(errno));
            throw std::system_error();
        }
    }
    syslog(LOG_INFO, "Semaphore timedWait succeeded. Specified seconds: %d", secondsToWait);
    return true;
}
