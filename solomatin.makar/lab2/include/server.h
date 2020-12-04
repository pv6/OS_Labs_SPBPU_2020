#pragma once
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <map>
#include "date.h"
#include "connection.h"

class Server {
    Date date;

    Server();
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;

    pthread_mutex_t nextClientMutex;
    int nextClient;
public:
    static Server &instance() {
        static Server server;
        return server;
    }

    static void handleSignal(int signum, siginfo_t* info, void* ptr);
    static void *response(void *pid);

    void parse(int argc, char *argv[]);
    void start();

    int getNextClient();

    ~Server();
};