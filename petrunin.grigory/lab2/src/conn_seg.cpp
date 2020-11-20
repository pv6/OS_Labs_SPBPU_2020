#include "../include/conn_seg.h"
#include "../include/custom_exception.h"

#include <syslog.h>    // syslog
#include <sys/types.h> // key_t
#include <sys/ipc.h>   // ftok, IPC_CREAT, IPC_RMID
#include <sys/shm.h>   // shmget, shmat, shmdt, shmctl
#include <cstring>     // memcpy

Conn* Conn::createConn(std::string const& working_dir, size_t max_buff_size) {
    return new ConnSeg(working_dir, max_buff_size);
}

ConnSeg::ConnSeg(std::string const& working_dir, size_t max_buff_size) :
    Conn(working_dir, max_buff_size) {
    syslog(LOG_DEBUG, "attempting to generate key");
    key_t shm_key = ftok(working_dir.c_str(), 1);
    if (shm_key == -1) {
        throw CustomException("ftok() error", __FILE__, __LINE__, (int)errno);
    }
    syslog(LOG_DEBUG, "attempting to create shared memory segment");
    m_id = shmget(shm_key, m_max_buff_size, IPC_CREAT | 0600);
    if (m_id == -1) {
        throw CustomException("shmget() error", __FILE__, __LINE__, (int)errno);
    }
}

ConnSeg::~ConnSeg() {
    if (m_ptr != (void*)-1) {
        close();
    }
}

void ConnSeg::open() {
    syslog(LOG_DEBUG, "attempting to attach shared memory segment");
    if (m_id == -1) {
        throw CustomException("no shared memery segment ID found", __FILE__, __LINE__);
    }
    m_ptr = shmat(m_id, 0, 0);
    if (m_ptr == (void*)-1) {
        throw CustomException("shmat() error", __FILE__, __LINE__, (int)errno);
    }
}

void ConnSeg::close() {
    syslog(LOG_DEBUG, "attempting to detach shared memory segment");
    if (m_ptr == (void*)-1) {
        throw CustomException("no shared memory segment attached", __FILE__, __LINE__);
    }
    if (shmdt(m_ptr) == -1) {
        throw CustomException("shmdt() error", __FILE__, __LINE__, (int)errno);
    }
    m_ptr = (void*)-1;
}

void ConnSeg::clear() {}

void ConnSeg::unlink() {
    syslog(LOG_DEBUG, "attempting to remove shared memory segment");
    if (shmctl(m_id, IPC_RMID, NULL) == -1) {
        throw CustomException("shmctl() error", __FILE__, __LINE__, (int)errno);
    }
}

void ConnSeg::read(void* buff, size_t size) const {
    syslog(LOG_DEBUG, "attempting to read from shared memory segment");
    validate(buff, size);
    memcpy(buff, m_ptr, size);
}

void ConnSeg::write(void* buff, size_t size) const {
    syslog(LOG_DEBUG, "attempting to write to shared memory segment");
    validate(buff, size);
    memcpy(m_ptr, buff, size);
}

void ConnSeg::validate(void* buff, size_t size) const {
    if (buff == nullptr) {
        throw CustomException("nullptr passed", __FILE__, __LINE__);
    }
    if (size > m_max_buff_size) {
        throw CustomException("inappropriate size", __FILE__, __LINE__);
    }
    if (m_ptr == nullptr || m_ptr == (void*)-1) {
        throw CustomException("no shared memory segment attached", __FILE__, __LINE__);
    }
}
