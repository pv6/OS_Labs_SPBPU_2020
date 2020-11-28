#ifndef LAB2_MY_CLIENT_H
#define LAB2_MY_CLIENT_H

#include <semaphore.h>
#include <syslog.h>
#include <iostream>
#include <csignal>
#include <cstring>
#include <semaphore.h>
#include <random>
#include <fcntl.h>
#include <unistd.h>
#include <random>

#include "../connection/connect.h"
#include "../connection/message.h"

class my_client {
public:
    static my_client & get_instance(int host_pid);
    bool open_connection();
    void run();
    my_client(const my_client &) = delete;
    my_client& operator=(const my_client &) = delete;

private:
    my_client(int host_pid);
    int host_pid;
    static void signal_handler(int sig_pid, siginfo_t *info, void *context);
    void terminate(int sig_pid);
    sem_t *host_sem, *client_sem;
    connection conn;
};


#endif //LAB2_MY_CLIENT_H
