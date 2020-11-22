//
// Created by Evgenia on 13.11.2020.
//

#include "conn.h"

bool Conn::Open(int c_id, bool create) {
    umask(0000);
    is_created = create;
    connection_name = "/tmp/fifo" + std::to_string(id);
    id = c_id;

    if (is_created) {
        int res = mkfifo(connection_name.c_str(), 0777);
        if (res == -1) {
            syslog(LOG_ERR, "Could not create fifo");
            syslog(LOG_ERR, "%s", strerror(errno));
        }
    }
    descr = open(connection_name.c_str(), O_RDWR);
    if (descr == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
        return false;
    }
    syslog(LOG_NOTICE, "FIFO connection is set");
    return true;
}

bool Conn::Read(void* buf, size_t count) {
    if (read(descr, buf, count) == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::Write(void* buf, size_t count) {
    if (write(descr, buf, count) == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::Close() {
    close(descr);
    if (is_created) {
        remove(connection_name.c_str());
    }
    syslog(LOG_NOTICE, "Fifo connection is closed");
    return true;
}

Conn::Conn () = default;
Conn::~Conn () = default;
