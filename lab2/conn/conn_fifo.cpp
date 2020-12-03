//
// Created by yudzhinnsk on 11/25/2020.
//

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>

#include "Conn.h"

#define FIFO_PATH "/tmp/l2_fifo"

bool Conn::CreateConn(size_t id, bool create)
{

    _fileDescr = new int(-1);
    _isCreated = create;
    _connName = id;

    if (_isCreated && mkfifo((FIFO_PATH + std::to_string(_connName)).c_str(), 0777) == -1) {
        std::cout << "ERROR: mkfifo failed - " << strerror(errno) << std::endl;
        return false;
    } else {
        *_fileDescr = open((FIFO_PATH + std::to_string(_connName)).c_str(), O_RDWR);
        if (*_fileDescr == -1) {
            std::cout << "ERROR: open failed - " << strerror(errno) << std::endl;
            return false;
        }
    }
    return true;
}

bool Conn::Read(void *buf, size_t count)
{
    if (read(*_fileDescr, buf, count) == -1) {
        std::cout << "ERROR: reading failed - " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Conn::Write(void *buf, size_t count)
{
    if (write(*_fileDescr, buf, count) == -1) {
        std::cout << "ERROR: writing failed: " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Conn::CloseConn(){
    if (close(*_fileDescr) < 0 || (_isCreated && remove((FIFO_PATH + std::to_string(_connName)).c_str()) < 0)) {
        std::cout << "ERROR: close failed - " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}