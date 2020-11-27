//
// Created by yudzhinnsk on 27.11.2020.
//

#ifndef LASTTRY_CONN_H
#define LASTTRY_CONN_H


#include <stdlib.h>
#include <string>

class Conn {
public:
    bool CreateConn(size_t id, bool create);
    bool CloseConn();

    bool Read(void *buf, size_t count);
    bool Write(void *buf, size_t count);

    Conn()= default;
    ~Conn()= default;

private:
    int  _connName;
    bool _isCreated;
    int* _fileDescr;
};


#endif //LASTTRY_CONN_H
