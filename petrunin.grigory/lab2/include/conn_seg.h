#ifndef VIA8_CONN_SEG_H
#define VIA8_CONN_SEG_H

#include "conn.h"

#include <string> // string

// shmget (shared memory segment)
class ConnSeg final : public Conn {
private:
    int m_id = -1;
    void* m_ptr = (void*)-1;

    ConnSeg(ConnSeg const&) = delete;
    ConnSeg& operator=(ConnSeg const&) = delete;

    void validate(void* buff, size_t size) const override;

public:
    ConnSeg(std::string const& working_dir, size_t max_buff_size);
    void open() override;
    void close() override;
    void clear() override;
    void unlink() override;
    void read(void* buff, size_t size) const override;
    void write(void* buff, size_t size) const override;
    ~ConnSeg() override;
};

#endif /* VIA8_CONN_SEG_H */
