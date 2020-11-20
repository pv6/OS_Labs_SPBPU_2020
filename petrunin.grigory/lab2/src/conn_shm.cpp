#include "../include/conn_shm.h"
#include "../include/custom_exception.h"

#include <sys/stat.h>
#include <sys/mman.h> // shm_open, shm_close, sh_unlink, mmap, munmap, PROT_READ, PROT_WRITE, MAP_SHARED
#include <syslog.h>   // syslog
#include <fcntl.h>    // O_CREAT
#include <unistd.h>   // ftruncate
#include <cstring>    // memcpy

std::string const ConnShm::name = "/lab2_shm";

Conn* Conn::createConn(std::string const& working_dir, size_t max_buff_size) {
    return new ConnShm(working_dir, max_buff_size);
}

ConnShm::ConnShm(std::string const& working_dir, size_t max_buff_size) :
    Conn(working_dir, max_buff_size) {}

ConnShm::~ConnShm() {
    // whatever
    if (m_fd != -1 && m_ptr != (void*)-1) {
        close();
    }
}

void ConnShm::open() {
    syslog(LOG_DEBUG, "attempting to open/create shared file");
    m_fd = shm_open(ConnShm::name.c_str(), O_CREAT | O_RDWR, 0600);
    if (m_fd == -1) {
        throw CustomException("shm_open() error", __FILE__, __LINE__, (int)errno);
    }
    syslog(LOG_DEBUG, "attempting to truncate shared file");
    if (ftruncate(m_fd, m_max_buff_size) != 0) {
        throw CustomException("ftruncate() error", __FILE__, __LINE__, (int)errno);
    }
    syslog(LOG_DEBUG, "attempting to map shared file");
    m_ptr = mmap(NULL, m_max_buff_size, PROT_READ | PROT_WRITE, MAP_SHARED, m_fd, 0);
    if (m_ptr == (void*)-1) {
        throw CustomException("mmap() error", __FILE__, __LINE__, (int)errno);
    }
}

void ConnShm::close() {
    syslog(LOG_DEBUG, "attempting to unmap shared file");
    if (m_ptr == (void*)-1) {
        throw CustomException("no shared memory mapped", __FILE__, __LINE__);
    }
    if (munmap(m_ptr, m_max_buff_size) != 0) {
        throw CustomException("munmap() error", __FILE__, __LINE__, (int)errno);
    }
    m_ptr = (void*)-1;
    if (::close(m_fd) != 0) {
        throw CustomException("close() error", __FILE__, __LINE__, (int)errno);
    }
    m_fd = -1;
}

void ConnShm::clear() {}

void ConnShm::unlink() {
    syslog(LOG_DEBUG, "attempting to unlink shared file");
    if (shm_unlink(ConnShm::name.c_str()) != 0) {
        throw CustomException("unlink() error", __FILE__, __LINE__, (int)errno);
    }
}

void ConnShm::read(void* buff, size_t size) const {
    syslog(LOG_DEBUG, "attempting to read from shared file");
    validate(buff, size);
    memcpy(buff, m_ptr, size);
}

void ConnShm::write(void* buff, size_t size) const {
    syslog(LOG_DEBUG, "attempting to write from shared file");
    validate(buff, size);
    memcpy(m_ptr, buff, size);
}

void ConnShm::validate(void* buff, size_t size) const {
    if (buff == nullptr) {
        throw CustomException("nullptr passed", __FILE__, __LINE__);
    }
    if (size > m_max_buff_size) {
        throw CustomException("inappropriate size", __FILE__, __LINE__);
    }
    if (m_fd == -1) {
        throw CustomException("no shared file opened", __FILE__, __LINE__);
    }
    if (m_ptr == nullptr || m_ptr == (void*)-1) {
        throw CustomException("no shared file mapped", __FILE__, __LINE__);
    }
}
