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

    const double sleepTime = 0.5;
    const double timeout = 10;

    pthread_mutex_t clientsMutex;
    std::map<int, Connection *> clients;

    pthread_mutex_t pidMutex;
    int pid = -1;

    Server();
    Server(const Server &) = delete;
    Server &operator=(const Server &) = delete;
    void removeConnection(int pid) {
        pthread_mutex_lock(&clientsMutex);
        if (clients.find(pid) != clients.end()) {
            delete clients[pid];
            clients.erase(pid);
        }
        pthread_mutex_unlock(&clientsMutex);
    }
    void addConnection(int pid, Connection *connection) {
        pthread_mutex_lock(&clientsMutex);
        clients[pid] = connection;
        pthread_mutex_unlock(&clientsMutex);
    }
public:
    static Server &instance() {
        static Server server;
        return server;
    }

    static void handleSignal(int signum, siginfo_t* info, void* ptr);
    static void *response(void *pid);

    bool parseDate(int argc, char *argv[]);
    void start();

    int nextClient() { static int i; return i++; }
    ~Server();
};