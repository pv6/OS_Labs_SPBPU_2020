#include <stdexcept>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <IConnection.h>

void IConnection::openConnection(size_t id, bool create) {
    owner = create;
    int shmflg = 0666;
    if (owner) {
        shmflg |= IPC_CREAT;
    }
    this->id = shmget(id, sizeof(DTO), shmflg);
    if (this->id == -1) {
        throw std::runtime_error("shmget failed, error " + std::string(strerror(errno)));
    }
}

void IConnection::readConnection(DTO *buf, size_t size) const {
    DTO * buf2 = (DTO *)shmat(id, nullptr, 0);
    memcpy(buf,buf2,size);
    if (buf == (DTO *) -1) {
        throw std::runtime_error("reading error " + std::string(strerror(errno)));
    }
    shmdt(buf);
}

void IConnection::writeConnection(DTO *buf, size_t size) const {
    DTO *shm_buf;
    shm_buf = (DTO *) shmat(id, nullptr, 0);
    if (shm_buf == (DTO *) -1) {
        throw std::runtime_error("writting error +" + std::string(strerror(errno)));
    }
    memcpy(shm_buf, buf, size);
    shmdt(shm_buf);
}

void IConnection::closeConnection() {
    if (owner && shmctl(id, IPC_RMID, nullptr) < 0) {
        throw std::runtime_error("close error +" + std::string(strerror(errno)));
    }
}
