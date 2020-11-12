#include "SemWrapper.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <stdexcept>
#include <cstring>

SemWrapper::SemWrapper(const std::string& semName) {
    sem = sem_open(semName.c_str(), O_CREAT, 0666, 0);
    if (sem == SEM_FAILED)
	validate(-1, "sem_open");
}

SemWrapper::~SemWrapper() {
    sem_destroy(sem);
}

void SemWrapper::wait() {
    int rc;
    while ((rc = sem_wait(sem)) == EINTR);  // loop
    validate(rc, "wait");
}

void SemWrapper::timedWait() {
    struct timespec ts; // for timeouts
    int rc = clock_gettime(CLOCK_REALTIME, &ts);
    if (rc == -1)
	throw std::runtime_error("Can't get current time for sem_timedwait");
    ts.tv_sec += timeoutSec;
    while ((rc = sem_timedwait(sem, &ts)) == EINTR);
    validate(rc, "timedWait");
}

void SemWrapper::post() {
    sem_post(sem);
}

void SemWrapper::validate(const int rc, const std::string& caller) const {
    if (rc != -1)
	return; // no error
    if (errno == ETIMEDOUT)
	    throw std::runtime_error("sem timed wait: timeout expired");
    throw std::runtime_error(caller + " returned error, errno = " + std::strerror(errno));
}
