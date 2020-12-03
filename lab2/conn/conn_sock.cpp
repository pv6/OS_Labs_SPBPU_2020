//
// Created by yudzhinnsk on 11/25/2020.
//
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <iostream>
#include "Conn.h"
#define SOCK_PATH "/tmp/lab2_ser"

bool Conn::CreateConn(size_t id, bool create)
{
    _fileDescr = new int[3];
    _connName = id;

    struct sockaddr_un saddr;
    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, (SOCK_PATH + std::to_string(_connName)).c_str());

    _isCreated = !create;
    if (_fileDescr[0]) {
        if (_isCreated) {
            _fileDescr[2] = accept(_fileDescr[1], nullptr, nullptr);
            if (_fileDescr[2] == -1) {
                std::cout << "ERROR: [HOST]: failed to accept the socket - " << strerror(errno) << std::endl;
                close(_fileDescr[1]);
                unlink((SOCK_PATH + std::to_string(_connName)).c_str());
                return false;
            }
        } else {
            if (connect(_fileDescr[2], (struct sockaddr *) &saddr, SUN_LEN(&saddr)) == -1) {
                std::cout << "ERROR: [CLIENT]: failed to connect the socket - " << strerror(errno) << std::endl;
                close(_fileDescr[2]);
                return false;
            }
        }
        return true;
    }

    if (create) {
        _fileDescr[2] = -1;
        _fileDescr[1] = socket(AF_UNIX, SOCK_STREAM, 0);
        if (_fileDescr[1] == -1) {
            std::cout << "ERROR: [HOST]: Failed to create listener - " << strerror(errno) << std::endl;
            return false;
        }

        if (bind(_fileDescr[1], (struct sockaddr *) &saddr, SUN_LEN(&saddr)) < 0) {
            std::cout << "ERROR: [HOST]: Failed to bind listener - " << strerror(errno) << std::endl;
            close(_fileDescr[1]);
            return false;
        }

        if (listen(_fileDescr[1], 1) == -1) {
            std::cout << "ERROR: [HOST]: Failed to listen - " << strerror(errno) << std::endl;
            close(_fileDescr[1]);
            unlink((SOCK_PATH + std::to_string(_connName)).c_str());
            return false;
        }

        _fileDescr[2] = accept(_fileDescr[1], nullptr, nullptr);
        if (_fileDescr[2] == -1) {
            std::cout << "ERROR: [HOST]: Failed to accept the listener - " << strerror(errno) << std::endl;
            close(_fileDescr[1]);
            unlink((SOCK_PATH + std::to_string(_connName)).c_str());
            return false;
        }
    } else {
        _fileDescr[1] = -1;
        _fileDescr[2] = socket(AF_UNIX, SOCK_STREAM, 0);
        if (_fileDescr[2] == -1) {
            std::cout << "ERROR: [CLIENT]: Failed to create socket - " << strerror(errno) << std::endl;
            return false;
        }

        if (connect(_fileDescr[2], (struct sockaddr *) &saddr, SUN_LEN(&saddr)) == -1) {
            std::cout << "ERROR: [CLIENT]: Failed to connect socket - " << strerror(errno) << std::endl;
            close(_fileDescr[2]);
            return false;
        }
    }
    _fileDescr[0] = true;
    return true;
}

bool Conn::CloseConn(){
    if (_fileDescr[0] || _fileDescr[1] != -1) {
        if (_fileDescr[2] != -1) {
            if (close(_fileDescr[2]) == -1) {
                std::cout << "ERROR: Failed to close sock - " << strerror(errno) << std::endl;
                return false;
            }
        }

        if (_fileDescr[1] != -1) {
            if (close(_fileDescr[1]) == -1) {
                std::cout << "ERROR: Failed to close listener - " << strerror(errno) << std::endl;
                return false;
            }
        }

        if (!_isCreated || (_isCreated && unlink((SOCK_PATH + std::to_string(_connName)).c_str()) == 0)) {
            std::cout << "Connection closed" << std::endl;
            _fileDescr[0] = false;
            return true;
        }

        std::cout << "ERROR: Connection closing failed - " << strerror(errno) << std::endl;
    }

    return true;
}

bool Conn::Write(void *buf, size_t count)
{
    if (recv(_fileDescr[2], buf, count, 0) == -1) {
        std::cout << "ERROR: failed to read message - " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool Conn::Read(void *buf, size_t count)
{
    if (send(_fileDescr[2], buf, count, MSG_NOSIGNAL) == -1) {
        std::cout << "ERROR: failed to send message - " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}