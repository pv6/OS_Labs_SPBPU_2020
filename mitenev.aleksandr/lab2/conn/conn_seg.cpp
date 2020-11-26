//
// Created by aleksandr on 26.11.2020.
//

#include "conn.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <cstring>

bool Conn::openConnection(size_t id, bool create) {

    owner = create;

    int shmflag = 0666;
    if (owner) {
        shmflag |= IPC_CREAT;
    }

    if ((this->id = shmget(id, sizeof(Message), shmflag)) == -1) {
        syslog(LOG_ERR, "shmget failed with error: %s", strerror(errno));
        return false;
    }

    return true;
}

bool Conn::readConnection(Message* buf, size_t size) const {
    Message* ptr = (Message *)shmat(id, nullptr, 0);
    if (ptr == (Message *)-1) {
        syslog(LOG_ERR, "shmat failed with error: %s", strerror(errno));
        return false;
    }
    *((Message *)buf) = *ptr;

    shmdt(ptr);
    return true;
}

bool Conn::writeConnection(Message *buf, size_t size) const {

    Message* ptr = (Message *)shmat(id, nullptr, 0);

    if (ptr == (Message *)-1) {
        syslog(LOG_ERR, "shmat failed with error: %s", strerror(errno));
        return false;
    }

    *ptr = *((Message *)buf);
    shmdt(ptr);

    return true;
}

bool Conn::closeConnection() {
    if (owner && shmctl(id, IPC_RMID, nullptr) < 0) {
        syslog(LOG_ERR, "shmctl failed with error: %s", strerror(errno));
        return false;
    }
    return true;
}