#ifndef VIA8_CONN_SHM_H
#define VIA8_CONN_SHM_H

#include "conn.h"

#include <string> // string

// shm_open + mmap (shared file)
class ConnShm final : public Conn {
private:
    static std::string const name;

    int m_fd = -1;
    void* m_ptr = (void*)-1;

    ConnShm(ConnShm const&) = delete;
    ConnShm& operator=(ConnShm const&) = delete;

    void validate(void* buff, size_t size) const override;

public:
    ConnShm(std::string const& working_dir, size_t max_buff_size);
    void open() override;
    void close() override;
    void clear() override;
    void unlink() override;
    void read(void* buff, size_t size) const override;
    void write(void* buff, size_t size) const override;
    ~ConnShm() override;
};

#endif /* VIA8_CONN_SHM_H */
