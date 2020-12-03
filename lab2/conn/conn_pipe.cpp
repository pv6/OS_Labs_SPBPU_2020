//
// Created by yudzhinnsk on 11/25/2020.
//
#include "Conn.h"
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>


bool Conn::CreateConn(size_t id, bool create) {
    if (create) {
        _fileDescr = (int *) new int[2];
        int fd[2];
        if (pipe(fd) == -1) {
            syslog(LOG_ERR, "Error in pipe establishment: %s", strerror(errno));
            return false;
        }
        _fileDescr[0] = fd[0];
        _fileDescr[1] = fd[1];
    }
    return true;
}

bool Conn::CloseConn() {
    if (::close(_fileDescr[0]) < 0 && ::close(_fileDescr[1]) < 0) {
        free(_fileDescr);
        return false;
    }
    free(_fileDescr);
    return true;
}

bool Conn::Read(void *buf, size_t count) {
    if (::read(_fileDescr[0], buf, count) <= 0) {
        syslog(LOG_ERR, "Error in reading: %s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::Write(void *buf, size_t count) {
    if (::write(_fileDescr[1], buf, count) <= 0) {
        syslog(LOG_ERR, "Error in writing: %s", strerror(errno));
        return false;
    }
    return true;
}