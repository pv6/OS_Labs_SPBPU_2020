//
// Created by Evgenia on 08.11.2020.
//

#include "conn.h"

bool Conn::Open(int c_id, bool create) {
    id = c_id;
    connection_name = "/shm" + std::to_string(c_id);
    int fd = shm_open(connection_name.c_str(), O_RDWR | O_CREAT, 0666);
    if (fd != -1) {
        ftruncate(fd, sizeof(int));
        f_descr = (int*) mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (f_descr == MAP_FAILED) {
            syslog(LOG_ERR, "Mmap failed");
            return false;
        }
        syslog(LOG_NOTICE, "SHM connection is set");
        return true;
    }
    syslog(LOG_ERR, "SHM: file descriptor is -1");
    return false;
}

bool Conn::Read(void* buf, size_t count) {
    return memcpy(buf, f_descr, count) != nullptr;
}

bool Conn::Write(void* buf, size_t count) {
    return memcpy(f_descr, buf, count) != nullptr;
}

bool Conn::Close() {
    munmap(f_descr, sizeof(int));
    shm_unlink(connection_name.c_str());
    syslog(LOG_NOTICE, "SHM connection is closed");
    return true;
}

Conn::Conn () = default;
Conn::~Conn () = default;
