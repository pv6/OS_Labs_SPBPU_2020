#include <fcntl.h>
#include <mqueue.h>
#include <syslog.h>
#include <cstring>

#include "conn.h"

bool Conn::open(int c_id, bool create) {
    isCreated = create;
    name = "/mq" + std::to_string(id);
    id = c_id;

    if (isCreated) {
        struct mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 10;
        attr.mq_msgsize = sizeof(int);
        attr.mq_curmsgs = 0;
        descr = mq_open(name.c_str(), O_RDWR | O_CREAT, 0666, &attr);
    } else {
        descr = mq_open(name.c_str(), O_RDWR);
    }
    if (descr == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
        return false;
    }
    syslog(LOG_NOTICE, "MQ connection is set");
    return true;
}

bool Conn::read(void* buf, size_t count) {
    if (mq_receive(descr, static_cast<char *>(buf), count, nullptr) == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::write(void* buf, size_t count) {
    if (mq_send(descr, (char *) buf, count, 0) == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::close() {
    if (!mq_close(descr)) {
        syslog(LOG_NOTICE, "MQ connection is closed");
        return (!isCreated || (!mq_unlink(name.c_str())));
    }
    syslog(LOG_ERR, "MQ connection closing errors");
    return false;
}
