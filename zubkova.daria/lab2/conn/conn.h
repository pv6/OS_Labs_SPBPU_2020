
#ifndef LAB2_CONN_H
#define LAB2_CONN_H

#include <cstdlib>
#include "../host/message.h"
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <semaphore.h>
#include <syslog.h>

class conn
{
public:
    bool Open(size_t id, bool create);
    bool Read(void *buf, size_t count);
    bool Write(void *buf, size_t count);
    bool Close();
private:
    bool create;
    size_t myId;
    int *fd;
    int filedectr;
    std::string name;

};
#endif //LAB2_CONN_H
