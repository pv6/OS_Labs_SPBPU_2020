//
// Created by Evgenia on 08.11.2020.
//

#include "conn.h"


bool Conn::Open(int c_id, bool create) {
    is_created = create;
    connection_name = "/mq" + std::to_string(id);
    id = c_id;

    if (is_created) {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 10;
        attr.mq_msgsize = sizeof(int);
        attr.mq_curmsgs = 0;
        descr = mq_open(connection_name.c_str(), O_RDWR | O_CREAT, 0666, &attr);
    } else {
        descr = mq_open(connection_name.c_str(), O_RDWR);
    }
    if (descr == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
        return false;
    }
    syslog(LOG_NOTICE, "MQ connection is set");
    return true;
}

bool Conn::Read(void* buf, size_t count) {
    if (mq_receive(descr, (char *) buf, count, nullptr) == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::Write(void* buf, size_t count) {
    if (mq_send(descr, (char *) buf, count, 0) == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::Close() {
    if (!mq_close(descr)) {
        syslog(LOG_NOTICE, "MQ connection is closed");
        return (!is_created || (!mq_unlink(connection_name.c_str())));
    }
    syslog(LOG_ERR, "MQ connection closing errors");
    return false;
}

Conn::Conn () = default;
Conn::~Conn () = default;
