#include <mqueue.h>
#include <cstring>

#include "../interfaces/Conn.h"
#include "../interfaces/Message.h"

#define MQ_PATH "/lab2_mq"

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

    int mq_flg = O_RDWR;
    if (own_) {
        std::cout << "Creating connection with id: " << id << std::endl;
        mq_flg |= O_CREAT;
        struct mq_attr attr = ((struct mq_attr) {0, 1, sizeof(Message), 0, {0}});
        *desc_ = mq_open((MQ_PATH + std::to_string(id_)).c_str(), mq_flg, 0666, &attr);
    } else {
        std::cout << "Getting connection with id: " << id << std::endl;
        *desc_ = mq_open((MQ_PATH + std::to_string(id_)).c_str(), mq_flg);
    }
    if (*desc_ == -1) {
        std::cout << "ERROR: mq_open failed - " << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

bool Conn::connReceive(void* buf, size_t count)
{
    if (mq_receive(*desc_, (char *) buf, count, nullptr) == -1) {
        std::cout << "ERROR: mq_recieve failed - " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool Conn::connSend(void* buf, size_t count)
{
    if (mq_send(*desc_, (char *) buf, count, 0) == -1) {
        std::cout << "ERROR: mq_send failed - " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool Conn::connClose()
{
    if (mq_close(*desc_) == 0) {
        if (!own_ || (mq_unlink((MQ_PATH + std::to_string(id_)).c_str()) == 0)) {
            return true;
        }
    }
    return false;
}
