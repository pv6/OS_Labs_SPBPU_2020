#include <syslog.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <cstring>

#include "../interface/conn.h"


Conn::Conn(int id, bool create) : id(id), create(create)
{
    int shmflg = S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP | S_IWOTH | S_IROTH;

    if (create)
        shmflg |= IPC_CREAT;

    file_desc = shmget(id, sizeof(int), shmflg);

    if (file_desc == -1)
        throw MyException("Shmget failed");
    
    if (create)
        syslog(LOG_INFO, "Host opened a connection.");
    else
        syslog(LOG_INFO, "Client connected to connection.");
}

void Conn::Write(int num)
{
    void *shmaddr = shmat(file_desc, nullptr, 0);
    
    if (shmaddr == (void*)(-1))
        throw MyException("Shmat failed");

    memcpy(shmaddr, (const char*)(&num), sizeof(int));

    if (shmdt(shmaddr) == -1)
        throw MyException("Shmdt failed");
}

int Conn::Read()
{
    int num;

    void *shmaddr = shmat(file_desc, nullptr, 0);
    
    if (shmaddr == (void*)(-1))
        throw MyException("Shmat failed");

    memcpy((char*)(&num), shmaddr, sizeof(int));

    if (shmdt(shmaddr) == -1)
        throw MyException("Shmdt failed");
    
    return num;
}

Conn::~Conn()
{
    if (create)
    {
        shmctl(file_desc, IPC_RMID, nullptr);
    
        syslog(LOG_INFO, "Host closed a connection.");
        syslog(LOG_INFO, "Host completed.");
    }
}
