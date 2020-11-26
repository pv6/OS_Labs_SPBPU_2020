//
// Created by Daria on 11/23/2020.
//

#include "conn.h"
#include "message.h"
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <iostream>

static std::string shm_name = "/shm";
bool conn::open(size_t id, bool create) {
    is_created_ = create;
    conn_name_ = shm_name + std::to_string(id);
    int mode = 0666;
    int fd;
    if (is_created_) {
        fd = shm_open(conn_name_.c_str(), O_CREAT | O_RDWR, mode);
        if (fd == -1) {
            syslog(LOG_ERR, "Error in opening shared memory: %s", strerror(errno));
            return false;
        }
        ftruncate(fd, sizeof(message));
    } else {
        fd = shm_open(conn_name_.c_str(), O_RDWR, mode);
        if (fd == -1) {
            syslog(LOG_ERR, "Error in opening shared memory: %s", strerror(errno));
            return false;
        }
    }
    file_descr_ = (int*)mmap(0, sizeof(message), PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
    if (file_descr_ == MAP_FAILED) {
        syslog(LOG_ERR, "ERROR: mmap failed, error = %s", strerror(errno));
        return false;
    }
    return true;
}

bool conn::close() {
    munmap(file_descr_, sizeof(message));
    shm_unlink(conn_name_.c_str());
    syslog(LOG_INFO, "Shm_memory connection closed");
    return true;
}

bool conn::read(void *buf, size_t count) {
    if (count > sizeof(message)) {
        syslog(LOG_ERR, "Incorrect size of message");
        return false;
    }
    return (memcpy(buf, file_descr_, count) != nullptr);
}

bool conn::write(void *buf, size_t count) {
    if (count > sizeof(message)) {
        syslog(LOG_ERR, "Incorrect size of message");
        return false;
    }
    return (memcpy(file_descr_, buf, count) != nullptr);
}
