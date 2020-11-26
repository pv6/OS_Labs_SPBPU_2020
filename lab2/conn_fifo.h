#ifndef CONN_FIFO_H_INCLUDED
#define CONN_FIFO_H_INCLUDED

#include "conn.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <string>

class ConnFifo:Conn{
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

#endif // CONN_FIFO_H_INCLUDED
