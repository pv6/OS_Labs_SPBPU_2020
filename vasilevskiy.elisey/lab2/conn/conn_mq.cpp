#include <fcntl.h>
#include <stdexcept>
#include <string>
#include <cstring>
#include <mqueue.h>
#include <IConnection.h>

void IConnection::openConnection(size_t id, bool create) {
    owner = create;
    name = "/LAB2_QUEUE";
    this->id = -1;
    int mqflg = O_RDWR;
    if (owner) {
        const int mqperm = 0666;
        mqflg |= O_CREAT;
        mq_attr attr;
        attr.mq_flags = 0;
        attr.mq_maxmsg = 1;
        attr.mq_curmsgs = 0;
        attr.mq_msgsize = sizeof(DTO);
        this->id = mq_open(name.c_str(), mqflg, mqperm, &attr);
    } else {
        this->id = mq_open(name.c_str(), mqflg);
    }
    if (this->id == -1) {
        throw std::runtime_error("mq_open failed, error " + std::string(strerror(errno)));
    }
}

void IConnection::readConnection(DTO *buf, size_t size) const{
    if (mq_receive(id, reinterpret_cast<char*>(buf), size, nullptr) == -1) {
        throw std::runtime_error("reading error " + std::string(strerror(errno)));
    }
}

void IConnection::writeConnection(DTO *buf, size_t size) const{
    if (mq_send(id, reinterpret_cast<char*>(buf), size, 0) == -1) {
        throw std::runtime_error("writting error +" + std::string(strerror(errno)));
    }
}

void IConnection::closeConnection() {
    if (mq_close(id) == 0) {
        if (owner && mq_unlink(name.c_str()) != 0) {
            throw std::runtime_error("close error +" + std::string(strerror(errno)));
        }
    } else {
        throw std::runtime_error("close error +" + std::string(strerror(errno)));
    }
}
