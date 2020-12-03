#ifndef CONN_H_INCLUDED
#define CONN_H_INCLUDED

#include <errno.h>
#include <iostream>
#include <stdio.h>
#include <syslog.h>
#include <stdexcept>
#include <string>
#include <cstring>
#include <cstddef>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include <sys/mman.h>


class Conn{
public:
    void openConn(bool isCreator);
    void readConn(char *buf, size_t count);
    void writeConn(char *buf, size_t count);
    void closeConn();
private:
    size_t msgSize = 11; //msg string is: dd.mm.yyyy'/0'
    bool isCreator;
    int id;
    static std::string name_path;
    char* ptr_map = nullptr;
};

#endif // CONN_H_INCLUDED
