#ifndef LAB2_GOATINFORMATION_H
#define LAB2_GOATINFORMATION_H

#include <semaphore.h>
#include <fcntl.h>
#include <cstring>

#include "../interfaces/Conn.h"

class GoatInformation {
public:
    bool openConnection(int id);

    void free();

    GoatInformation(int pid = 0, int id = 0);

    void attach(int pid) { pid_ = pid; isAttached_ = true; }

    void detach() { isAttached_ = false; }

    bool isAttached() { return isAttached_; }

    int getId() { return id_; }

    int getPid() { return pid_; }

    Conn &getConnection() { return connection_; }

    sem_t &getHostSemaphore() { return *semaphoreHost_; }

    sem_t &getClientSemaphore() { return *semaphoreClient_; }

    int countStepsDead_;
private:
    int pid_;
    bool isAttached_;
    int id_;
    Conn connection_;
    sem_t* semaphoreHost_;
    sem_t* semaphoreClient_;
};


#endif //LAB2_GOATINFORMATION_H
