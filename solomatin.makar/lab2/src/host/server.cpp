#include <iostream>
#include <string.h>
#include <set>
#include <pthread.h>
#include "server.h"
#include "connection.h"
#include "print_utils.h"

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
    printOk("Received SIGUSR1 from " + std::to_string(pid));

    if (server.clients.find(pid) != server.clients.end()) {
        printOk("Client with pid " + std::to_string(pid) + " sent signal but already exists");
        return;
    }

    printOk("New client arrived with pid " + std::to_string(pid));

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, response, new int{pid});
}

void *Server::response(void *pidPointer) {
    Server &server = Server::instance();

    int pid = *(int *)pidPointer;

    printOk("Entered thread function for pid " + std::to_string(pid));

    delete (int *)pidPointer;
    int id = server.nextClient();

    try {
    Connection *connection = Connection::create(id);

    printOk("Connection for pid " + std::to_string(pid) + " created", id);

    server.addConnection(pid, connection);

    // notify client that channel created
    sem_wait(connection->clientSemaphore);
        sigval sv;
        sv.sival_int = id;
        sigqueue(pid, SIGUSR1, sv);
        printOk("Sent signal back to " + std::to_string(pid) + " that channel created", id);

        connection->write((char *)&server.date, sizeof(Date));
    sem_post(connection->clientSemaphore);

    printOk("Waiting on server semaphore...", id);
    sem_wait(connection->serverSemaphore);
    int prediction;
    connection->read((char *)&prediction, sizeof(prediction));
    printOk("Client relesed semaphore, prediction read: " + std::to_string(prediction), id);

    server.removeConnection(pid);
    printOk("Connection with id " + std::to_string(id) + " deleted", id);

    fflush(stdout);
    } catch (const char *error) {
        printErr("Error while processing client" + std::string(error), id);
    }
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
    printOk("Started server with date " + date.toString());
    fflush(stdout);

    while(true) {
        sleep(1000);
    }
}

Server::~Server() {
    for (auto const &pair : clients) delete pair.second;
    pthread_mutex_destroy(&clientsMutex);
}