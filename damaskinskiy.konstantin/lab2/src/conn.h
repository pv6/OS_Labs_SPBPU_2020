#ifndef CONN_H
#define CONN_H

#include <cstddef>
#include <string>

class Conn
{
public:
    bool open( int pid, bool isCreated );
    bool close();
    bool read( void *buf, size_t count );
    bool write( void *buf, size_t count );

private:
    const int msgmaxlen = 11;
    bool isCreated;
    std::string name;

    int descr;
    int* f_descr;
};

#endif // CONN_H
