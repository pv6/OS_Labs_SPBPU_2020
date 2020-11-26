//
// Created by aleksandr on 26.11.2020.
//

#include "conn.h"
#include <mqueue.h>
#include <cstring>

bool Conn::openConnection(size_t id, bool create) {
    owner = create;
    name = "/LAB2_QUEUE";

    int mqflg = O_RDWR;
    int mqperm = 0666;

    if (owner) {
        mq_attr attr;

        mqflg |= O_CREAT;

        attr.mq_maxmsg = 1;
        attr.mq_msgsize = sizeof(Message);
        attr.mq_curmsgs = 0;
        attr.mq_flags=0;
        this->id = mq_open(name.c_str(), mqflg, mqperm, &attr);
    }
    else {
        this->id = mq_open(name.c_str(), mqflg);
    }

    if (this->id == -1) {
        syslog(LOG_ERR, "mq_open failed with error: %s", strerror(errno));
        return false;
    }

    return true;
}

bool Conn::readConnection(Message *buf, size_t size) const {
    if (mq_receive(id, (char *)buf, size, nullptr) == -1) {
        syslog(LOG_ERR, "mq_recieve failed with error: %s", strerror(errno));
        return false;
    }

    return true;
}

bool Conn::writeConnection(Message *buf, size_t size) const {
    if (mq_send(id, (char*)buf, size, 0) == -1) {
        syslog(LOG_ERR, "mq_send failed with error: %s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::closeConnection() {

    if (mq_close(id) != 0) {
        syslog(LOG_ERR, "mq_close failed with error: %s", strerror(errno));
        return false;
    }

    if (owner && mq_unlink(name.c_str()) != 0) {
        syslog(LOG_ERR, "mq_unlink failed with error: %s", strerror(errno));
        return false;
    }

    return true;
}