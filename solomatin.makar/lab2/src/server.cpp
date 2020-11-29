#include <iostream>
#include <string.h>
#include <set>
#include <pthread.h>
#include "server.h"
#include "connection.h"

Server::Server() : date{0,0,0} {
    struct sigaction act;
    act.sa_sigaction = handleSignal;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, nullptr);

    pthread_mutex_init(&clientsMutex, nullptr);
}

void Server::handleSignal(int signum, siginfo_t* info, void* ptr) {
    Server &server = Server::instance();
    int pid = info->si_pid;
    std::cout << "Received SIGUSR1 from " << pid << std::endl;

    if (server.clients.find(pid) != server.clients.end()) {
        std::cout << "Client with pid " << pid << "sent signal second time, closing connection" << std::endl;
        server.removeConnection(pid);
        return;
    }

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&tid, &attr, response, nullptr);
}

void *Server::response(void *pidPointer) {
    Server &server = Server::instance();

    int pid = *(int *)pidPointer;
    Connection *connection = new Connection(server.nextClient());
    server.addConnection(pid, connection);

    // notify client that channel created
    union sigval sv;
    sv.sival_int = connection->getId();
    sigqueue(pid, SIGUSR1, sv);

    char *buf = server.date.serialize();
    connection->write(buf, sizeof(Date));
    delete buf;

    return nullptr;
}

bool Server::parseDate(int argc, char *argv[]) {
    static const char *help = "Usage: ./server [--date YEAR MONTH DAY]\n";
    if (argc == 1) {
        srand(time(0));
        date = {rand() % 10 + 2010, rand() % 12, rand() % 28};
    } else if (argc == 5 && !strcmp(argv[1], "--date")) {
        date = {atoi(argv[2]), atoi(argv[3]), atoi(argv[4])};
    } else {
        std::cout << help << std::endl;
        return false;
    }
    return true;
}

void Server::start() {
    std::cout << "Started server with date " << date.toString() << std::endl;
    fflush(stdout);

    while(true) {
        sleep(sleepTime);
        for (auto const &pair : clients) {
            if (pair.second->getLifetime() > timeout) {
                std::cout << "Client with pid " << pair.first <<
                " do not respond for " << timeout << " seconds, removing it" << std::endl;
                removeConnection(pair.first);
            }
        }
    }
}

Server::~Server() {
    for (auto const &pair : clients) delete pair.second;
    pthread_mutex_destroy(&clientsMutex);
}