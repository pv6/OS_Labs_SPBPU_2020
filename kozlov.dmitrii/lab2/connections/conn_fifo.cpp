#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <unistd.h>

#include "conn.h"

Conn::Conn(int host_pid_, bool create) {
    openlog("GameConnection", LOG_PID, LOG_DAEMON);
    _owner = create;
    _hostPid = host_pid_;
    std::string filename = std::string("/tmp/" + std::to_string(_hostPid)).c_str();


    if (_owner) {
        unlink(filename.c_str());
        int res = mkfifo(filename.c_str(), 0777);
        if (res == -1) {
            throw std::runtime_error("mkfifo creation error");
        }
    }

    if ((_desc = open(filename.c_str(), O_RDWR)) == -1) {
        throw std::runtime_error("open mkfifo file error");
    }

    if (_owner) {
        syslog(LOG_INFO, "fifo created with name %s", filename.c_str());
        std::cout << "fifo created with name " << filename << std::endl;
    } else {
        syslog(LOG_INFO, "fifo opened with name %s", filename.c_str());
        std::cout << "fifo opened with name " << filename << std::endl;
    }
}

Conn::~Conn() {
    std::string filename = std::string("/tmp/" + std::to_string(_hostPid)).c_str();

    if (_owner) {
        unlink(filename.c_str());
        syslog(LOG_INFO, "close fifo with name  %s", filename.c_str());
        std::cout << "close fifo with name " << filename << std::endl;
    }
    closelog();
}

bool Conn::Read(void* buf, size_t count) noexcept {
    if (read(_desc, buf, count) == -1) {
        perror("read() ");
        return false;
    }

    return true;
}

bool Conn::Write(void* buf, size_t count) noexcept {
    if (write(_desc, buf, count) == -1) {
        perror("write() ");
        return false;
    }

    return true;
}
