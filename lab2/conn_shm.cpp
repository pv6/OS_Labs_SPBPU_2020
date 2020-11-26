#include "conn_shm.h"


void ConnFifo::openConn() {
    int flg = 0600;
    int shmflag = O_RDWR;
    if (isCreator)
        shmflg |= O_CREAT;
    this->id = shm_open(ConnShm::name.c_str(), shmflag, flg);
    if (this->id == -1)
        throw std::runtime_error("shm_open failed, error " + std::string(strerror(errno)));
}

void ConnFifo::readConn(char *buf, size_t count) {
    if (buf == nullptr)
	    throw std::runtime_error("nullptr buf passed into shm reading");
    if (count != msgSize)
	    throw std::runtime_error("wrong msg size, it's should be 10: dd.mm.yyyy");
    char* buf_r = (char*)mmap(NULL, count, PROT_READ | PROT_WRITE, MAP_SHARED, id, 0);
    memcpy(buf, buf_r, count);
    if (buf == (char*) -1)
        throw std::runtime_error("reading error " + std::string(strerror(errno)));
    if (munmap(buf, count) != 0)
        throw std::runtime_error("unmap error " + std::string(strerror(errno)));
}

void ConnFifo::writeConn(char *buf, size_t count) {
    if (buf == nullptr)
	    throw std::runtime_error("nullptr buf passed into shm writing");
    if (count != msgSize)
	    throw std::runtime_error("wrong msg size, it's should be 10: dd.mm.yyyy");
    char* shm_buf;
    buf_w = (char*)mmap(NULL, count, PROT_READ | PROT_WRITE, MAP_SHARED, id, 0);
    if (buf_w == (char*) -1)
        throw std::runtime_error("writting error +" + std::string(strerror(errno)));
    memcpy(buf_w, buf, count);
    if (buf_w == (char*) -1)
        throw std::runtime_error("writting error +" + std::string(strerror(errno)));
    if (munmap(buf, count) != 0)
        throw std::runtime_error("unmap error " + std::string(strerror(errno)));
}

void ConnFifo::closeConn() {
    if (close(id) != 0)
        throw std::runtime_error("error in closing " + std::string(strerror(errno)));
}
