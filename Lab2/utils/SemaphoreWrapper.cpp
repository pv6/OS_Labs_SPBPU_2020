#include "SemaphoreWrapper.h"
#include "MyException.h"

SemaphoreWrapper::SemaphoreWrapper(const std::string &shm_name) : shm_name(shm_name) {
    int shm = openShm();
    sem = initSem(shm);
}

int SemaphoreWrapper::openShm()
{
    int shm = shm_open(shm_name.c_str(), O_RDWR | O_CREAT, S_IRWXU);

    if (shm == -1)
        throw MyException("Failed allocation shared memory");

    return shm;
}

sem_t* SemaphoreWrapper::initSem(int shm)
{
    if (ftruncate(shm, sizeof(sem_t)) == -1)
        throw MyException("Ftruncate failed");

    sem_t *sem = (sem_t*)mmap(nullptr, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);

    if (sem == MAP_FAILED)
        throw MyException("Mmap failed");

    if (sem_init(sem, 1, 0) == -1)
        throw MyException("Sem_init failed");

    return sem;
}

void SemaphoreWrapper::post()
{
    if (sem_post(sem) == -1)
        throw MyException("Sem_post error.");
}

void SemaphoreWrapper::timedWait()
{
    struct timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
        throw MyException("Clock_gettime failed");

    ts.tv_sec += TIMEOUT;

    if (sem_timedwait(sem, &ts) == -1) {
        if (errno != ETIMEDOUT)
            throw MyException("sem_timedwait failed");
        else
            throw MyException("Response timeout");
    }
}

void SemaphoreWrapper::wait()
{
    if (sem_wait(sem) == -1)
        throw MyException("Sem_wait failed");
}

SemaphoreWrapper::~SemaphoreWrapper()
{
    sem_destroy(sem);
    shm_unlink(shm_name.c_str());
}
