#ifndef LAB2_MY_HOST_H
#define LAB2_MY_HOST_H
#include <semaphore.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <syslog.h>
#include <unistd.h>
#include "../connection/connect.h"
#include "../connection/message.h"
#include <errno.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>
#include <cstdlib>

class my_host
{
public:
    static my_host & get_instance();
    bool open_connection();
    void run();
    my_host(const my_host &) = delete;
    my_host& operator=(const my_host &) = delete;
private:

    my_host();
    bool get_msg(message &msg);
    void terminate();
    static void signal_handler(int sig, siginfo_t *info, void *context);

    sem_t *host_sem, *client_sem;
    connection conn;
    bool is_client_attached;
    int client_pid;
};

#endif //LAB2_MY_HOST_H
