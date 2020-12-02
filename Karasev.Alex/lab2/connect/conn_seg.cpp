#include <stdexcept>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "IConnection.h"


void IConnection::openConnection(size_t id, bool create) {
    owner = create;
    int shmflg = 0666;
    if (owner) {
        shmflg |= IPC_CREAT;
    }
    this->id = shmget(id, sizeof(DateAndTemp), shmflg);
    if (this->id == -1) {
        throw std::runtime_error("shmget error:" + std::string(strerror(errno)));
    }
}

void IConnection::readConnection(DateAndTemp *buf, size_t size) const {
    auto* buf2 = (DateAndTemp*) shmat(id, nullptr, 0);
    memcpy(buf,buf2,size);
    if (buf == (DateAndTemp *) -1) {
        throw std::runtime_error("shmat reading error:" + std::string(strerror(errno)));
    }
    shmdt(buf);
}

void IConnection::writeConnection(DateAndTemp *buf, size_t size) const {
    DateAndTemp *shm_buf;
    shm_buf = (DateAndTemp *) shmat(id, nullptr, 0);
    if (shm_buf == (DateAndTemp *) -1) {
        throw std::runtime_error("shmat writing error:" + std::string(strerror(errno)));
    }
    memcpy(shm_buf, buf, size);
    shmdt(shm_buf);
}

void IConnection::closeConnection() const {
    if (owner && shmctl(id, IPC_RMID, nullptr) < 0) {
        throw std::runtime_error("shmclt error:" + std::string(strerror(errno)));
    }
}