#include "conn_seg.h"

void ConnFifo::openConn() {
    int shmflg = 0666;
    if (isCreator)
        shmflg |= IPC_CREAT;
    this->id = shmget(id, sizeof(DTO), shmflg);
    if (this->id == -1)
        throw std::runtime_error("shmget failed, error " + std::string(strerror(errno)));
}

void ConnFifo::readConn(char *buf, size_t count) {
    if (buf == nullptr)
	    throw std::runtime_error("nullptr buf passed into seg reading");
    if (count != msgSize)
	    throw std::runtime_error("wrong msg size, it's should be 10: dd.mm.yyyy");
    char* buf_r = (char *)shmat(id, nullptr, 0);
    memcpy(buf, buf_r, count);
    if (buf == (char*) -1) {
        throw std::runtime_error("reading error " + std::string(strerror(errno)));
    }
    shmdt(buf);
}

void ConnFifo::writeConn(char *buf, size_t count) {
    if (buf == nullptr)
	    throw std::runtime_error("nullptr buf passed into seg writing");
    if (count != msgSize)
	    throw std::runtime_error("wrong msg size, it's should be 10: dd.mm.yyyy");
    char* shm_buf;
    buf_w = (char*)shmat(id, nullptr, 0);
    if (buf_w == (char*) -1)
        throw std::runtime_error("writting error +" + std::string(strerror(errno)));
    memcpy(buf_w, buf, count);
    shmdt(buf_w);
}

void ConnFifo::closeConn() {
    if (isCreator && shmctl(id, IPC_RMID, nullptr) < 0)
        throw std::runtime_error("close error +" + std::string(strerror(errno)));
}
