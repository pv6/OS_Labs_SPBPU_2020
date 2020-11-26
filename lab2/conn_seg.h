#ifndef CONN_SEG_H_INCLUDED
#define CONN_SEG_H_INCLUDED

#include "conn.h"
#include <stdexcept>
#include <cstring>
#include <sys/ipc.h>
#include <sys/shm.h>

class ConnSeg:Conn{
public:
    void openConn() override;
    void readConn(char *buf, size_t count) override;
    void writeConn(char *buf, size_t count) override;
    void closeConn() override;
    virtual ~ConnFifo(){}
    ConnFifo(bool isCreator){this->isCreator = isCreator;}
private:
    size_t msgSize = 11; //msg string is: dd.mm.yyyy'/0'
    bool isCreator;
    int id;
    const std::string name_path = "/tmp/fifo.txt";
};

#endif // CONN_SEG_H_INCLUDED
