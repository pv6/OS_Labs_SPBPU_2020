#include "../include/conn_fifo.h"
#include "../include/custom_exception.h"

#include <sys/stat.h>
#include <sys/types.h> // mkfifo
#include <syslog.h>    // syslog
#include <fcntl.h>     // open, close
#include <unistd.h>    // read, write
#include <errno.h>     // errno, EEXIST

std::string const ConnFifo::filename = "ipc.fifo";

Conn* Conn::createConn(std::string const& working_dir, size_t max_buff_size) {
    return new ConnFifo(working_dir, max_buff_size);
}

ConnFifo::ConnFifo(std::string const& working_dir, size_t max_buff_size) :
    Conn(working_dir, max_buff_size), m_path(working_dir + ConnFifo::filename) {
    syslog(LOG_DEBUG, "attempting to make fifo");
    if (mkfifo(m_path.c_str(), 0600) != 0 && errno != EEXIST) {
        throw CustomException("mkfifo() error", __FILE__, __LINE__, (int)errno);
    }
}

ConnFifo::~ConnFifo() {
    if (m_fd != -1) {
        close();
    }
}

void ConnFifo::open() {
    syslog(LOG_DEBUG, "attempting to open fifo");
    m_fd = ::open(m_path.c_str(), O_RDWR);
    if (m_fd == -1) {
        throw CustomException("open() error", __FILE__, __LINE__, (int)errno);
    }
}

void ConnFifo::close() {
    syslog(LOG_DEBUG, "attempting to close fifo");
    if (m_fd == -1) {
        throw CustomException("no fifo opened", __FILE__, __LINE__);
    }
    if (::close(m_fd) != 0) {
        throw CustomException("close() error", __FILE__, __LINE__, (int)errno);
    }
    m_fd = -1;
}

// this is required in case client exited
// unexpectedly and didn't read from fifo
// (otherwise new client would read old data)
void ConnFifo::clear() {
    syslog(LOG_DEBUG, "attempting to clear fifo");
    if (m_fd == -1) {
        throw CustomException("no fifo opened", __FILE__, __LINE__);
    }
    if (::close(m_fd) != 0) {
        throw CustomException("close() error", __FILE__, __LINE__, (int)errno);
    }
    char buff[32];
    while (::read(m_fd, buff, 32) == 32);
    m_fd = ::open(m_path.c_str(), O_RDWR);
    if (m_fd == -1) {
        throw CustomException("open() error", __FILE__, __LINE__, (int)errno);
    }
    syslog(LOG_DEBUG, "clearing fifo complete");
}

void ConnFifo::unlink() {
    syslog(LOG_DEBUG, "attempting to unlink fifo");
    if (::unlink(m_path.c_str()) != 0) {
        throw CustomException("unlink() error", __FILE__, __LINE__, (int)errno);
    }
}

void ConnFifo::read(void* buff, size_t size) const {
    syslog(LOG_DEBUG, "attempting to read from fifo");
    validate(buff, size);
    if (::read(m_fd, buff, size) != (int)size) {
        throw CustomException("read() error", __FILE__, __LINE__, (int)errno);
    }
}

void ConnFifo::write(void* buff, size_t size) const {
    syslog(LOG_DEBUG, "attempting to write to fifo");
    validate(buff, size);
    if (::write(m_fd, buff, size) != (int)size) {
        throw CustomException("write() error", __FILE__, __LINE__, (int)errno);
    }
}

void ConnFifo::validate(void* buff, size_t size) const {
    if (buff == nullptr) {
        throw CustomException("nullptr passed", __FILE__, __LINE__);
    }
    if (size > m_max_buff_size) {
        throw CustomException("inappropriate size", __FILE__, __LINE__);
    }
    if (m_fd == -1) {
        throw CustomException("no fifo opened", __FILE__, __LINE__);
    }
}
