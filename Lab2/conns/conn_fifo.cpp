

#include <syslog.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "../interface/conn.h"

const char* FIFO_PATH_NAME = "fifo";

Conn::Conn(int id, bool create) : id(id), create(create)
{
    if (create)
        if (mkfifo(FIFO_PATH_NAME, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH | S_IWOTH) == -1)
            throw MyException("Mkfifo failed");
    
    file_desc = open(FIFO_PATH_NAME, O_RDWR);
    
    if (file_desc == -1)
        throw MyException("Open failed");
    
    if (create)
        syslog(LOG_INFO, "Host opened a connection.");
    else
        syslog(LOG_INFO, "Client connected to connection.");
}

void Conn::Write(int num)
{
    if (write(file_desc, &num, sizeof(int)) == -1)
        throw MyException("Write failed");
}

int Conn::Read()
{
    int num;

    if (read(file_desc, &num, sizeof(int)) == -1)
        throw MyException("Read failed");
    
    return num;
}

Conn::~Conn()
{
    if (create)
    {
        close(file_desc);
        unlink(FIFO_PATH_NAME);

        syslog(LOG_INFO, "Host closed a connection");
        syslog(LOG_INFO, "Host completed");
    }
}
