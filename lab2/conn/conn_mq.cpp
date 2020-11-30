//
// Created by Daria on 11/23/2020.
//
#include "conn.h"
#include "message.h"
#include <cerrno>
#include <mqueue.h>
#include <syslog.h>
#include <cstring>

static std::string mq_name = "/mq";
bool conn::open(size_t id, bool create) {
    file_descr_ = (int*)malloc(sizeof(int));
    is_created_ = create;
    conn_name_ = mq_name + std::to_string(id);
    int mode = 0666;
    if (is_created_) {
        struct mq_attr attr = {0, 1, sizeof(message), 0, {0}};
        (*file_descr_) = mq_open(conn_name_.c_str(), O_RDWR | O_CREAT, mode, &attr);
    } else {
        (*file_descr_) = mq_open(conn_name_.c_str(), O_RDWR);
    }
    if (*file_descr_ == -1) {
        syslog(LOG_ERR, "Cannot establish connection with id = %zu, error = %s", id, strerror(errno));
        return false;
    }
    syslog(LOG_INFO, "Connection with id %lu established successfully", id);
    return true;
}

bool conn::close() {
    if (!mq_close(*file_descr_)) {
        free(file_descr_);
        syslog(LOG_INFO, "Connection is closed");
        return (!is_created_ || !(mq_unlink(conn_name_.c_str())));
    }
    syslog(LOG_ERR, "Connection closing error");
    return false;
};

bool conn::read(void *buf, size_t count) {
    if (mq_receive(*file_descr_, (char *) buf, count, nullptr) == -1) {
        syslog(LOG_ERR, "Problems with reading: %s", strerror(errno));
        return false;
    }
    return true;
}

bool conn::write(void *buf, size_t count) {
    if (mq_send(*file_descr_, (char *) buf, count, 0) == -1) {
        syslog(LOG_ERR, "Problems with writing: %s", strerror(errno));
        return false;
    }
    return true;
}

