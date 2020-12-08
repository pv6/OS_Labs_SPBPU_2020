#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>

#include "../interfaces/Conn.h"

#define FIFO_PATH "/tmp/lab2_fifo"

Conn::Conn () {
    desc_ = new int(-1);
}

Conn::~Conn () {
    delete desc_;
}

bool Conn::connOpen(size_t id, bool create)
{
    own_ = create;
    id_ = id;

    if (own_ && mkfifo((FIFO_PATH + std::to_string(id_)).c_str(), 0777) == -1) {
        std::cout << "ERROR: mkfifo failed - " << strerror(errno) << std::endl;
        return false;
    } else {
        *desc_ = open((FIFO_PATH + std::to_string(id_)).c_str(), O_RDWR);
        if (*desc_ == -1) {
            std::cout << "ERROR: open failed - " << strerror(errno) << std::endl;
            return false;
        }
    }
    return true;
}

bool Conn::connReceive(void* buf, size_t count)
{
    if (read(*desc_, buf, count) == -1) {
        std::cout << "ERROR: reading failed - " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Conn::connSend(void* buf, size_t count)
{
    if (write(*desc_, buf, count) == -1) {
        std::cout << "ERROR: writing failed: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Conn::connClose()
{
    if (close(*desc_) < 0 || (own_ && remove((FIFO_PATH + std::to_string(id_)).c_str()) < 0)) {
        std::cout << "ERROR: close failed - " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}


