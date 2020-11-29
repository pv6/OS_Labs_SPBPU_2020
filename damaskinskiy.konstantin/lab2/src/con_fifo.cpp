#include <cstring>
#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "conn.h"

bool Conn::open(int id, bool create) {
    umask(0000);
    isCreated = create;
    name = "/tmp/fifo_DK_predictor_" + std::to_string(id);

    if (isCreated) {
        int res = mkfifo(name.c_str(), 0777);
        if (res == -1) {
            syslog(LOG_ERR, "Could not create fifo");
            syslog(LOG_ERR, "%s", strerror(errno));
        }
    }
    descr = ::open(name.c_str(), O_RDWR);
    if (descr == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
        return false;
    }
    syslog(LOG_INFO, "FIFO connection is set");
    return true;
}

bool Conn::read(void* buf, size_t count) {
    if (::read(descr, buf, count) == -1) {
        syslog(LOG_ERR, "FIFO error: %s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::write(void* buf, size_t count) {
    if (::write(descr, buf, count) == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
        return false;
    }
    return true;
}

bool Conn::close() {
    ::close(descr);
    if (isCreated) {
        remove(name.c_str());
    }
    syslog(LOG_INFO, "FIFO connection is closed");
    return true;
}
