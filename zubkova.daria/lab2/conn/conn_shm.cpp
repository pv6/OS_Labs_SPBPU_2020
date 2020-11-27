
#include "conn.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <map>


bool conn::Open(size_t id, bool create) {
    this->create = create;
    myId = id;
    name = "/lab2_shm" + std::to_string(id);
    int shm = shm_open(name.c_str(), O_RDWR | O_CREAT, 0666);
    if (shm != -1) {
        ftruncate(shm, sizeof(int));
        fd = (int*) mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm, 0);
        if (fd == MAP_FAILED) {
            //std::cout << "ERROR: mmap failed, error = " << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: mmap failed, error = %s", strerror(errno));
            return false;
        }
        //std::cout << "SHM connection is created with id = " << id << std::endl;
        syslog(LOG_NOTICE, "SHM connection is creates with id =  %i", (int)id);
        return true;
    }
    //std::cout << "Error on opening connection for client " << id << std::endl;
    syslog(LOG_ERR, "SHM: file descriptor is -1");
    return false;
}

bool conn::Read(void *buf, size_t count) {
    if (memcpy(buf, fd, count) != nullptr)
        return true;
    //std::cout << "ERROR: read failed with error = " << strerror(errno) << std::endl;
    syslog(LOG_ERR, "ERROR: read failed with error = %s", strerror(errno));
    return false;
}

bool conn::Write(void *buf, size_t count) {
    if (memcpy(fd, buf, count) != nullptr)
        return true;
    //std::cout << "ERROR: write failed with error = " << strerror(errno) << std::endl;
    syslog(LOG_ERR, "ERROR: write failed with error = %s", strerror(errno));
    return false;
}

bool conn::Close() {
    munmap(fd, sizeof(int));
    shm_unlink(name.c_str());
    syslog(LOG_NOTICE, "Connection close");
    return true;
}