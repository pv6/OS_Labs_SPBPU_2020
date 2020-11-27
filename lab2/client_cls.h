#ifndef CLIENT_CLS_H_INCLUDED
#define CLIENT_CLS_H_INCLUDED

#include <semaphore.h>
#include <syslog.h>
#include <iostream>
#include <csignal>
#include <cstring>
#include <random>
#include <fcntl.h>
#include <unistd.h>
#include <random>
#include <stdexcept>
#include <map>
#include "conn.h"
#include "DTStor.h"
#include "ConnHelper.h"

class Client{
public:
    static Client &getInstance(int host_pid);
    void openConn();
    void termClient();
    void startClient();
private:
    Client(int host_pid);
    void ClientWait();
    bool isWork = true;
    Conn connection;
    int pid;
    static void handleSignal(int signum);
    sem_t *host_semaphore;
    sem_t *client_semaphore;
};

#endif // CLIENT_CLS_H_INCLUDED
