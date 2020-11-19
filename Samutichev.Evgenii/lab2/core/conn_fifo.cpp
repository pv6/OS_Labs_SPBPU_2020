#include "conn.h"
#include "sys_exception.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>
#include <syslog.h>
#include <errno.h>
#include <unistd.h>

const char* prefix = "fifo_";

namespace {
    class ConnectionImpl : public Connection {
    public:
        ConnectionImpl(size_t id, bool create);
        int read() const override;
        void write(int msg) const override;

        ~ConnectionImpl() override;

    private:
        int _descr;
        std::string _name;
        bool _creator;
    };

    ConnectionImpl::ConnectionImpl(size_t id, bool create) {
        _name = prefix + std::to_string(id);
        _creator = create;

        if (_creator) {
            if (mkfifo(_name.c_str(), S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IWOTH) == -1)
                throw SysException("Failed to mkfifo (FIFO)", errno);
        }

        _descr = open(_name.c_str(), O_RDWR);
        if (_descr == -1)
            throw SysException("Failed to open fifo file (FIFO)", errno);
    }

    int ConnectionImpl::read() const {
        int msg;
        if (::read(_descr, &msg, sizeof(int)) == -1)
            throw SysException("Failed to read (FIFO)", errno);
        return msg;
    }

    void ConnectionImpl::write(int msg) const {
        if (::write(_descr, &msg, sizeof(int)) == -1)
            throw SysException("Failed to write (FIFO)", errno);
    }

    ConnectionImpl::~ConnectionImpl() {
        if (close(_descr) == -1)
            syslog(LOG_ERR, "An unhandled exception occured while closing fifo file %d (FIFO)", _descr);
        if (_creator && unlink(_name.c_str()) == -1)
            syslog(LOG_ERR, "An unhandled exception occured while unlinking fifo file %s (FIFO)", _name.c_str());
    }
}

Connection* Connection::create(size_t id, bool create) {
    return new ConnectionImpl(id, create);
}
