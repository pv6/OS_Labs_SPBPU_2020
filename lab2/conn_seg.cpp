#include "conn.h"

std::string Conn::name_path = "/tmp/lab2";

void Conn::openConn(bool isCreator) {
    this->isCreator = isCreator;
    int shmflg = 0666;
    key_t shm_key = ftok(Conn::name_path.c_str(), 1);
    if (shm_key == -1)
       throw std::runtime_error("shmkey failed, error " + std::string(strerror(errno)));
    if (isCreator)
        shmflg |= IPC_CREAT;
    this->id = shmget(shm_key, sizeof(char), shmflg);
    if (this->id == -1)
        throw std::runtime_error("shmget failed, error " + std::string(strerror(errno)));
    this->ptr_map = (char *)shmat(id, nullptr, 0);
}

void Conn::readConn(char *buf, size_t count) {
    if (buf == nullptr)
	    throw std::runtime_error("nullptr buf passed into seg reading");
    if (count > msgSize)
	    throw std::runtime_error("wrong msg size, it's should be 10: dd.mm.yyyy");
    memcpy(buf, this->ptr_map, count - 1);
    if (buf == (char*) -1) {
        throw std::runtime_error("reading error " + std::string(strerror(errno)));
    }
}

void Conn::writeConn(char *buf, size_t count) {
    if (buf == nullptr)
	    throw std::runtime_error("nullptr buf passed into seg writing");
    if (count > msgSize)
	    throw std::runtime_error("wrong msg size, it's should be 10: dd.mm.yyyy");
    memcpy(this->ptr_map, buf, count - 1);
    if (this->ptr_map == (char*) -1)
        throw std::runtime_error("writting error +" + std::string(strerror(errno)));
}

void Conn::closeConn() {
    if (isCreator && shmctl(id, IPC_RMID, nullptr) < 0)
        throw std::runtime_error("close error +" + std::string(strerror(errno)));
    if (shmdt(this->ptr_map) == -1)
        throw std::runtime_error("close error +" + std::string(strerror(errno)));
}
