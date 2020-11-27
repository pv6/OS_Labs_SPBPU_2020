#include <syslog.h>
#include <mqueue.h>

#include "../interface/conn.h"

const char* MQ_NAME = "/mq";

Conn::Conn(int id, bool create) : id(id), create(create)
{
    if (create)
    {
        struct mq_attr mqa = { 0, 1, sizeof(int), 0 };
        file_desc = mq_open(MQ_NAME, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH, &mqa);
    }
    else
        file_desc = mq_open(MQ_NAME, O_RDWR);
    
    if (file_desc == -1)
        throw MyException("Mq_open failed");
    
    if (create)
        syslog(LOG_INFO, "Host opened a connection.");
    else
        syslog(LOG_INFO, "Client connected to connection.");
}

void Conn::Write(int num)
{
    if (mq_send(file_desc, (const char*)(&num), sizeof(int), 0) == -1)
        throw MyException("Mq_send failed");
}

int Conn::Read()
{
    int num;

    if (mq_receive(file_desc, (char*)(&num), sizeof(int), nullptr) == -1)
        throw MyException("Mq_receive failed");
    
    return num;
}

Conn::~Conn()
{
    if (create)
    {
        mq_close(file_desc);
        mq_unlink(MQ_NAME);
    
        syslog(LOG_INFO, "Host closed a connection.");
        syslog(LOG_INFO, "Host completed.");
    }
}
