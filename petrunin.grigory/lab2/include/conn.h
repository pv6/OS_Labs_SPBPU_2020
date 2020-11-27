#ifndef VIA8_CONN_H
#define VIA8_CONN_H

#include <string>   // string
#include <stddef.h> // size_t

class Conn {
private:
    Conn(Conn const&) = delete;
    Conn& operator=(Conn const&) = delete;

protected:
    std::string const m_working_dir;
    
    Conn(std::string const& working_dir, size_t max_buff_size) 
        : m_working_dir(working_dir), m_max_buff_size(max_buff_size) {}
    
    virtual void validate(void* buff, size_t size) const {}

public:
    size_t const m_max_buff_size;

    static Conn* createConn(std::string const& working_dir, size_t max_buff_size);
    virtual ~Conn() {}
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void clear() = 0;
    virtual void unlink() = 0;
    virtual void read(void* buff, size_t size) const = 0;
    virtual void write(void* buff, size_t size) const = 0;
};

#endif /* VIA8_CONN_H */
