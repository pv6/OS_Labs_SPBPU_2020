#ifndef HOST_CLS_H_INCLUDED
#define HOST_CLS_H_INCLUDED

#include <semaphore.h>
#include <syslog.h>
#include <iostream>
#include <csignal>
#include <cstring>
#include <random>
#include <fcntl.h>
#include <unistd.h>
#include <random>
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
    void startHost(const std::string& inputFile, const std::string& outputFile);
private:
    void HostWait();
    bool isWork = true;
    HostConnInfo hci;
    bool isWork = true;
    Conn connection;
    int pid;
    static void handleSignal(int sig, siginfo_t *info, void *context);
    sem_t *host_semaphore;
    sem_t *client_semaphore;
    Host();
};

#endif // HOST_CLS_H_INCLUDED
