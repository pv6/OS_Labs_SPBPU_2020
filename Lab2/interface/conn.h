#ifndef CONN_H
#define CONN_H

#include <unistd.h>
#include "../utils/SemaphoreWrapper.h"
#include "../utils/MyException.h"
#include <string>

class Conn
{
public:
    Conn(int id, bool c);
    void Write(int num);
    int Read();
    ~Conn();

private:
    int id;
    bool create;
    int file_desc;
};

#endif //CONN_H
