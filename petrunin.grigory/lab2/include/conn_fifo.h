#ifndef VIA8_CONN_FIFO_H
#define VIA8_CONN_FIFO_H

#include "conn.h"

#include <string> // string

// fifo (named pipe)
class ConnFifo final : public Conn {
private:
    static std::string const filename;
    
    std::string const m_path;
    int m_fd = -1;

    ConnFifo(ConnFifo const&) = delete;
    ConnFifo& operator=(ConnFifo const&) = delete;

    void validate(void* buff, size_t size) const override;

public:
    ConnFifo(std::string const& working_dir, size_t max_buff_size);
    void open() override;
    void close() override;
    void clear() override;
    void unlink() override;
    void read(void* buff, size_t size) const override;
    void write(void* buff, size_t size) const override;
    ~ConnFifo() override;
};

#endif /* VIA8_CONN_FIFO_H */
