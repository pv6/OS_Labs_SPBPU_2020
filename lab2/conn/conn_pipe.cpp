//
// Created by Daria on 11/23/2020.
//
#include "conn.h"
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>

bool conn::open(size_t id, bool create) {
    if (create) {
        file_descr_ = (int *) new int[2];
        int fd[2];
        if (pipe(fd) == -1) {
            syslog(LOG_ERR, "Error in pipe establishment: %s", strerror(errno));
            return false;
        }
        file_descr_[0] = fd[0];
        file_descr_[1] = fd[1];
    }
    return true;
}

bool conn::close() {
    if (::close(file_descr_[0]) < 0 && ::close(file_descr_[1]) < 0) {
        free(file_descr_);
        return false;
    }
    free(file_descr_);
    return true;
}

bool conn::read(void *buf, size_t count) {
    if (::read(file_descr_[0], buf, count) <= 0) {
        syslog(LOG_ERR, "Error in reading: %s", strerror(errno));
        return false;
    }
    return true;
}

bool conn::write(void *buf, size_t count) {
    if (::write(file_descr_[1], buf, count) <= 0) {
        syslog(LOG_ERR, "Error in writing: %s", strerror(errno));
        return false;
    }
    return true;
}