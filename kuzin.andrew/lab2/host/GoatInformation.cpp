#include "GoatInformation.h"

GoatInformation::GoatInformation(int pid, int id) {
    pid_ = pid;
    isAttached_ = (pid != 0);
    id_ = id;
    semaphoreHost_ = nullptr;
    semaphoreClient_ = nullptr;
    countStepsDead_ = 0;
}


bool GoatInformation::openConnection(int id)
{
    bool res = false;
    id_ = id;
    sem_unlink((SEMAPHORE_HOST_NAME + std::to_string(id_)).c_str());
    sem_unlink((SEMAPHORE_CLIENT_NAME + std::to_string(id_)).c_str());
    semaphoreHost_ = sem_open((SEMAPHORE_HOST_NAME + std::to_string(id_)).c_str(), O_CREAT, 0666, 0);
    semaphoreClient_ = sem_open((SEMAPHORE_CLIENT_NAME + std::to_string(id_)).c_str(), O_CREAT, 0666, 0);

    if (connection_.connOpen(id, true)) {
        if (semaphoreHost_ == SEM_FAILED || semaphoreClient_ == SEM_FAILED) {
            std::cout << "ERROR: sem_open failed: " << strerror(errno) << std::endl;
            res = false;
        } else {
            res = true;
        }
    }
    return res;
}


void GoatInformation::free()
{
    if (semaphoreHost_ == nullptr || semaphoreClient_ == nullptr)
        return;

    if (sem_unlink((SEMAPHORE_HOST_NAME + std::to_string(id_)).c_str()) == -1 ||
        sem_unlink((SEMAPHORE_CLIENT_NAME + std::to_string(id_)).c_str()) == -1) {
        std::cout << "Failed: " << strerror(errno) << std::endl;
    }
    if (!connection_.connClose()) {
        std::cout << "Failed: " << strerror(errno) << std::endl;
    }
    semaphoreClient_ = nullptr;
    semaphoreHost_ = nullptr;
}