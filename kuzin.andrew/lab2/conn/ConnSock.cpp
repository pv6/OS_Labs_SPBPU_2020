#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "../interfaces/Conn.h"

#define SOCK_PATH "/tmp/lab2_server"

Conn::Conn () {
    desc_ = new int[3];
}

Conn::~Conn () {
    delete[] desc_;
}

bool Conn::connOpen(size_t id, bool create)
{
    id_ = id;

    struct sockaddr_un saddr;
    saddr.sun_family = AF_UNIX;
    strcpy(saddr.sun_path, (SOCK_PATH + std::to_string(id_)).c_str());

    own_ = create;
    if (desc_[0]) {
        if (own_) {
            desc_[2] = accept(desc_[1], nullptr, nullptr);
            if (desc_[2] == -1) {
                std::cout << "ERROR: [HOST]: failed to accept the socket - " << strerror(errno) << std::endl;
                close(desc_[1]);
                unlink((SOCK_PATH + std::to_string(id_)).c_str());
                return false;
            }
        } else {
            if (connect(desc_[2], (struct sockaddr *) &saddr, SUN_LEN(&saddr)) == -1) {
                std::cout << "ERROR: [CLIENT]: failed to connect the socket - " << strerror(errno) << std::endl;
                close(desc_[2]);
                return false;
            }
        }
        return true;
    }

    if (create) {
        desc_[2] = -1;
        desc_[1] = socket(AF_UNIX, SOCK_STREAM, 0);
        if (desc_[1] == -1) {
            std::cout << "ERROR: [HOST]: Failed to create listener - " << strerror(errno) << std::endl;
            return false;
        }

        if (bind(desc_[1], (struct sockaddr *) &saddr, SUN_LEN(&saddr)) < 0) {
            std::cout << "ERROR: [HOST]: Failed to bind listener - " << strerror(errno) << std::endl;
            close(desc_[1]);
            return false;
        }

        if (listen(desc_[1], 1) == -1) {
            std::cout << "ERROR: [HOST]: Failed to listen - " << strerror(errno) << std::endl;
            close(desc_[1]);
            unlink((SOCK_PATH + std::to_string(id_)).c_str());
            return false;
        }

        desc_[2] = accept(desc_[1], nullptr, nullptr);
        if (desc_[2] == -1) {
            std::cout << "ERROR: [HOST]: Failed to accept the listener - " << strerror(errno) << std::endl;
            close(desc_[1]);
            unlink((SOCK_PATH + std::to_string(id_)).c_str());
            return false;
        }
    } else {
        desc_[1] = -1;
        desc_[2] = socket(AF_UNIX, SOCK_STREAM, 0);
        if (desc_[2] == -1) {
            std::cout << "ERROR: [CLIENT]: Failed to create socket - " << strerror(errno) << std::endl;
            return false;
        }

        if (connect(desc_[2], (struct sockaddr *) &saddr, SUN_LEN(&saddr)) == -1) {
            std::cout << "ERROR: [CLIENT]: Failed to connect socket - " << strerror(errno) << std::endl;
            close(desc_[2]);
            return false;
        }
    }
    desc_[0] = true;
    return true;
}

bool Conn::connClose()
{
    if (desc_[0] || desc_[1] != -1) {
        if (desc_[2] != -1) {
            if (close(desc_[2]) == -1) {
                std::cout << "ERROR: Failed to close sock - " << strerror(errno) << std::endl;
                return false;
            }
        }

        if (desc_[1] != -1) {
            if (close(desc_[1]) == -1) {
                std::cout << "ERROR: Failed to close listener - " << strerror(errno) << std::endl;
                return false;
            }
        }

        if (!own_ || (own_ && unlink((SOCK_PATH + std::to_string(id_)).c_str()) == 0)) {
            std::cout << "Connection closed" << std::endl;
            desc_[0] = false;
            return true;
        }

        std::cout << "ERROR: Connection closing failed - " << strerror(errno) << std::endl;
    }

    return true;
}

bool Conn::connReceive(void *buf, size_t count)
{
    if (recv(desc_[2], buf, count, 0) == -1) {
        std::cout << "ERROR: failed to read message - " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}

bool Conn::connSend(void *buf, size_t count)
{
    if (send(desc_[2], buf, count, MSG_NOSIGNAL) == -1) {
        std::cout << "ERROR: failed to send message - " << strerror(errno) << std::endl;
        return false;
    }

    return true;
}
