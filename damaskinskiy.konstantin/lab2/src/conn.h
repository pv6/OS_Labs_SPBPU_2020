#ifndef CONN_H
#define CONN_H

#include <cstddef>

class Conn
{
    Conn( int pid, bool create );
    bool read( void *buf, size_t count );
    bool write( void *buf, size_t count );
};

#endif // CONN_H
