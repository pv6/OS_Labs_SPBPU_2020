#ifndef HOST_CLS_H_INCLUDED
#define HOST_CLS_H_INCLUDED

#include <semaphore.h>
#include <syslog.h>
#include <iostream>
#include <fstream>
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
#include "Host_Conn_Inf.h"

class Host{
public:
    static Host &getInstance();
    void openConn();
    void termHost();
    void startHost(const char* inputFile, const char* outputFile);
private:
    void HostWait();
    bool isWork = true;
    HostConnInfo hci;
    Conn connection;
    int pid;
    static void signalHandler(int sig, siginfo_t *info, void *context);
    sem_t *host_semaphore{};
    sem_t *client_semaphore{};
    Host();
    Host(const Host&);
    Host& operator=(Host&);
};

#endif // HOST_CLS_H_INCLUDED