//
// Created by aleksandr on 26.11.2020.
//

#include <sys/stat.h>
#include <unistd.h>
#include "conn.h"
#include <cstring>

bool Conn::openConnection(size_t id, bool create) {
    owner = create;
    name = "/tmp/lab2_pipe";
    int fifoflag = 0777;

    if (owner && mkfifo(name.c_str(), fifoflag) == -1){
        syslog(LOG_ERR, "mkfifo failed with error: %s", strerror(errno));
        return false;
    }

    this->id = open(name.c_str(), O_RDWR);

    if (this->id == -1){
        syslog(LOG_ERR, "open failed with error: %s", strerror(errno));
        return false;
    }

    return true;
}

bool Conn::readConnection(Message* buf, size_t size) const {
    if (read(id, reinterpret_cast<char*>(buf), size) == -1){
        syslog(LOG_ERR, "reading failed with error: %s", strerror(errno));
        return false;
    }

    return true;
}

bool Conn::writeConnection(Message *buf, size_t size) const {
    if (write(id, reinterpret_cast<char*>(buf), size) == -1){
        syslog(LOG_ERR, "writing failed with error: %s", strerror(errno));
        return false;
    }

    return true;
}

bool Conn::closeConnection() {
    if (close(id) < 0 || (owner && remove(name.c_str()) < 0))
    {
        syslog(LOG_ERR, "close failed with error: %s", strerror(errno));
        return false;
    }
    return true;
}