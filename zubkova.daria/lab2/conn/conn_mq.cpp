
#include "conn.h"
#include <iostream>
#include <errno.h>
#include <fcntl.h>
#include <cstring>
#include <mqueue.h>
#include <map>

bool conn::Open(size_t id, bool create) {
    this->create = create;
    myId = id;
    name = "/lab2_mq" + std::to_string(id);
    if (create) {
        struct mq_attr attr = ((struct mq_attr) {0, 1, sizeof(Message), 0, {0}});
        filedectr = mq_open(name.c_str(), O_RDWR | O_CREAT, 0666, &attr);
    } else {
        filedectr = mq_open(name.c_str(), O_RDWR);
    }
    if (filedectr == -1) {
        syslog(LOG_ERR, "ERROR: mq failed, error = %s", strerror(errno));
        return false;
    }
    //std::cout << "MQ connection with id = " << id  << std::endl;
    syslog(LOG_NOTICE, "MQ connection with id = %i", (int)id);
    return true;
}

bool conn::Read(void *buf, size_t count) {
    if (mq_receive(filedectr, (char*)buf, count, nullptr) != -1){
        return true;
    }
    //std::cout << "ERROR: read failed with error = " << strerror(errno) << std::endl;
    syslog(LOG_ERR, "ERROR: read failed with error = %s", strerror(errno));
    return false;
}

bool conn::Write(void *buf, size_t count) {
    if (mq_send(filedectr, (char*)buf, count, 0) != -1){
        return true;
    }
    //std::cout << "ERROR: write failed with error = " << strerror(errno) << std::endl;
    syslog(LOG_ERR, "ERROR: write failed with error = %s", strerror(errno));
    return false;
}

bool conn::Close() {
    if (!mq_close(filedectr)) {
        syslog(LOG_NOTICE, "Connection close");
        return (!create || (!mq_unlink(name.c_str())));
    }
    syslog(LOG_ERR, "ERROR: close failed with error = %s", strerror(errno));
    return false;
}
