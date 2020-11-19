#ifndef CONN_H_INCLUDED
#define CONN_H_INCLUDED

#include <stdio.h>

class Connection {
public:
    static Connection* create(size_t id, bool create);
    virtual int read() const = 0;
    virtual void write(int msg) const = 0;

    virtual ~Connection() = default;

protected:
    Connection() = default;
};

#endif // CONN_H_INCLUDED
