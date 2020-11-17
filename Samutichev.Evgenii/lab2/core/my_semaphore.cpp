#include "my_semaphore.h"
#include "sys_exception.h"
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>

Semaphore::Semaphore(const char* name) {
    _name = name;

    int memDescr = shm_open(_name.c_str(), O_RDWR | O_CREAT, S_IRWXU);
    if (memDescr == -1)
        throw SysException("Failed to create shared memory", errno);

    if (ftruncate(memDescr, sizeof(sem_t)) == -1)
        throw SysException("Failed to truncate", errno);

    _sem = (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, memDescr, 0);
    if (_sem == MAP_FAILED)
        throw SysException("Failed to map", errno);

    if (sem_init(_sem, 1, 0) == -1)
        throw SysException("Failed to initialize semaphore", errno);
}

void Semaphore::wait() {
    syslog(LOG_NOTICE, "[%s] locked", _name.c_str());
    if (sem_wait(_sem) == -1)
        throw SysException("Failed on sem_wait", errno);
}

bool Semaphore::timedWait(time_t timeout) {
    syslog(LOG_NOTICE, "[%s] locked with timer", _name.c_str());
    struct timespec ts;
    timespec_get(&ts, TIMER_ABSTIME);
    ts.tv_sec += timeout;
    if (sem_timedwait(_sem, &ts) == -1) {
        if (errno != ETIMEDOUT)
            throw SysException("Failed to wait sem_timedwait", errno);
        else {
            return false;
        }
    }
    return true;
}

void Semaphore::post() {
    syslog(LOG_NOTICE, "[%s] unlocked", _name.c_str());
    if (sem_post(_sem) == -1)
        throw SysException("Failed on sem_post", errno);
}

Semaphore::~Semaphore() {
    sem_destroy(_sem);
    shm_unlink(_name.c_str());
}
