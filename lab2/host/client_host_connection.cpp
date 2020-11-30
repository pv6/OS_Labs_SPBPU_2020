//
// Created by Daria on 11/25/2020.
//

#include "client_host_connection.h"
#include <syslog.h>
#include <unistd.h>
#include <cstring>


bool client_host_connection::open(int id)  {
    is_closed_ = false;
    signalled_to = false;
    client_id = id;
    if (pipe(fd) == -1) {
        syslog(LOG_ERR, "Pipe creation failed: %s", strerror(errno));
        return false;
    }
    return true;
}

bool client_host_connection::disconnect() {
    if (close(fd[0]) < 0 && close(fd[1]) < 0) {
        syslog(LOG_ERR, "Disconnection of pipe failed: %s", strerror(errno));
        return false;
    }
    is_closed_ = true;
    return true;
}

bool client_host_connection::read(void *buf, size_t count) {
    if (::read(fd[0], buf, count) <= 0) {
        syslog(LOG_ERR, "Reading failed with error = %s", strerror(errno));
        return false;
    }
    return true;
}


bool client_host_connection::write(void *buf, size_t count) {
    if (::write(fd[1], buf, count) == -1) {
        syslog(LOG_ERR, "Writing failed with error = %s", strerror(errno));
        return false;
    }
    return true;
}

int client_host_connection::get_client_id() {
    return client_id;
}

void client_host_connection::signal_to() {
    this->signalled_to = true;
}

bool client_host_connection::has_signalled_to() const{
    return signalled_to;
}

