#include "conn.h"

std::string Conn::name_path = "/shm";

void Conn::openConn(bool isCreator) {
    this->isCreator = isCreator;
    int shmflag = O_RDWR;
    if (isCreator)
        shmflag |= O_CREAT;
    this->id = shm_open(Conn::name_path.c_str(), shmflag, 0600);
    if (this->id == -1)
        throw std::runtime_error("shm_open failed, error " + std::string(strerror(errno)));
    if (ftruncate(this->id, msgSize) != 0)
       std::cout << std::string(strerror(errno)) << std::endl;
    this->ptr_map = (char*)mmap(NULL, msgSize, PROT_READ | PROT_WRITE, MAP_SHARED, id, 0);
    if (this->ptr_map == nullptr || this->ptr_map == (void*)-1)
       throw std::runtime_error("shm_open failed, error " + std::string(strerror(errno)));
}

void Conn::readConn(char *buf, size_t count) {
    if (buf == nullptr)
	    throw std::runtime_error("nullptr buf passed into shm reading");
    if (count > msgSize)
	    throw std::runtime_error("wrong msg size, it's should be 10: dd.mm.yyyy");
    memcpy(buf, (char*)this->ptr_map, count - 1);
    if (buf == (char*) -1)
       throw std::runtime_error("reading error " + std::string(strerror(errno)));
}

void Conn::writeConn(char *buf, size_t count) {
    if (buf == nullptr)
	    throw std::runtime_error("nullptr buf passed into shm writing");
    if (count > msgSize)
	    throw std::runtime_error("wrong msg size, it's should be 10: dd.mm.yyyy");
    memcpy(this->ptr_map, buf, count - 1);
    if (this->ptr_map == (char*) -1)
       throw std::runtime_error("writting error +" + std::string(strerror(errno)));

}

void Conn::closeConn() {
    if (munmap(this->ptr_map, msgSize) != 0)
       throw std::runtime_error("unmap error " + std::string(strerror(errno)));
}
