#include "conn.h"
#include "sys_exception.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <string>
#include <syslog.h>
#include <unistd.h>

const char* prefix = "sock_";

namespace {
    class ConnectionImpl : public Connection {
    public:
        ConnectionImpl(size_t id, bool create);
        int read() const override;
        void write(int msg) const override;

        ~ConnectionImpl() override;
    private:
        int _descr1, _descr2;
        std::string _id;
        bool _creator;
    };

    ConnectionImpl::ConnectionImpl(size_t id, bool create) {
        _id = prefix + std::to_string(id);
        _creator = create;

        struct sockaddr_un addr;
        memset(&addr, 0, sizeof(struct sockaddr_un));
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, _id.c_str());

        if (_creator) {
            _descr1 = socket(AF_UNIX, SOCK_STREAM, 0);
            if (_descr1 == -1)
                throw SysException("Failed to create socket (SOCK)", errno);

            if (bind(_descr1, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
                throw SysException("Failed to bind address (SOCK)", errno);

            if (listen(_descr1, 1) == -1)
                throw SysException("Failed to listen for connections (SOCK)", errno);

            _descr2 = accept(_descr1, nullptr, nullptr);
            if (_descr2 == -1)
                throw SysException("Failed to accept connection (SOCK)", errno);

            syslog(LOG_NOTICE, "CONN 1");
        } else {
            _descr2 = socket(AF_UNIX, SOCK_STREAM, 0);
            if (_descr2 == -1)
                throw SysException("Failed to get socket (SOCK)", errno);

            if (connect(_descr2, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1)
                throw SysException("Failed to connect (SOCK)", errno);

            syslog(LOG_NOTICE, "CONN 2");
        }
    }

    int ConnectionImpl::read() const {
        int msg;

        if (recv(_descr2, &msg, sizeof(int), 0) == -1)
            throw SysException("Failed to recieve message (SOCK)", errno);

        return msg;
    }

    void ConnectionImpl::write(int msg) const {
        if (send(_descr2, &msg, sizeof(int), MSG_NOSIGNAL) == -1)
            throw SysException("Failed to send message (SOCK)", errno);
    }

    ConnectionImpl::~ConnectionImpl() {
        if (close(_descr2) == -1)
            syslog(LOG_ERR, "An unhandled exception occured while closing connected socket handle %d (SOCK)", _descr2);

        if (_creator) {
            if (close(_descr1) == -1)
                syslog(LOG_ERR, "An unhandled exception occured while closing original socket handle %d (SOCKET)", _descr1);

            unlink(_id.c_str());
        }
    }
}

Connection* Connection::create(size_t id, bool create) {
    return new (std::nothrow) ConnectionImpl(id, create);
}
