#include <iostream>
#include <string.h>
#include <set>
#include <fcntl.h>
#include <pthread.h>
#include "server.h"
#include "connection.h"
#include "print_utils.h"
#include "global_settings.h"

Server::Server() : date{0,0,0}, nextClient(0) {
    struct sigaction act;
    act.sa_sigaction = handleSignal;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, nullptr);

    pthread_mutex_init(&nextClientMutex, nullptr);
}

void Server::handleSignal(int signum, siginfo_t* info, void* ptr) {
    int pid = info->si_pid;
    printOk("Received SIGUSR1 from " + std::to_string(pid));

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, response, new int{pid});
}

void *Server::response(void *pidPointer) {
    Server &server = Server::instance();
    int pid = *(int *)pidPointer;
    delete (int *)pidPointer;
    int id = server.getNextClient();

    printOk("Entered response function", id);

    try {
        timespec t;
        if (clock_gettime(CLOCK_REALTIME, &t) == -1) {
            perror("clock_gettime");
            return nullptr;
        }
        t.tv_sec += TIMEOUT;

        Connection connection = Connection(id, true);
        printOk("Connection created", id);

        std::string clientSemaphoreName = CLIENT_SEMAPHORE + std::to_string(id);
        std::string serverSemaphoreName = SERVER_SEMAPHORE + std::to_string(id);
        if (sem_unlink(clientSemaphoreName.c_str()) == 0) {
            printOk("Semaphore " + clientSemaphoreName + " removed from system", id);
        }
        if (sem_unlink(serverSemaphoreName.c_str()) == 0) {
            printOk("Semaphore " + serverSemaphoreName + " removed from system", id);
        }
        sem_t *clientSemaphore = sem_open(clientSemaphoreName.c_str(), O_CREAT | O_EXCL, 0666, 1);
        sem_t *serverSemaphore = sem_open(serverSemaphoreName.c_str(), O_CREAT | O_EXCL, 0666, 1);

        if (clientSemaphore == SEM_FAILED || serverSemaphore == SEM_FAILED) {
            throw "Could not create semaphores";
        }
        printOk("Created semaphore " + clientSemaphoreName, id);
        printOk("Created semaphore " + serverSemaphoreName, id);

        sem_wait(clientSemaphore); // non-blocking wait

        sigval sv;
        sv.sival_int = id;
        sigqueue(pid, SIGUSR1, sv);
        printOk("Sent signal back to " + std::to_string(pid) + " that channel created", id);

        connection.accept();

        printOk("Writing date to channel...", id);
        if (!connection.write((char *)&server.date, sizeof(server.date))) {
            printErr("Could not write to channel, closing connection...");
            sem_close(serverSemaphore);
            sem_close(clientSemaphore);
            sem_unlink(serverSemaphoreName.c_str());
            sem_unlink(clientSemaphoreName.c_str());
            return nullptr;
        }
        printInfo("Writed date: " + server.date.toString(), id);

        sem_wait(serverSemaphore); // non-blocking wait
        sem_post(clientSemaphore);

        int prediction;
        printOk("Waiting when client post server semaphore...", id);
        while (sem_timedwait(serverSemaphore, &t) < 0) { // blocking wait
            if (errno == EINTR) continue;

            perror("sem_timedwait");
            sem_close(serverSemaphore);
            sem_close(clientSemaphore);
            sem_unlink(serverSemaphoreName.c_str());
            sem_unlink(clientSemaphoreName.c_str());
            return nullptr;
        }
        printOk("Client released server semaphore!", id);
        if (!connection.read((char *)&prediction, sizeof(prediction))) {
            printErr("Could not read from channel, closing connection...");
            sem_close(serverSemaphore);
            sem_close(clientSemaphore);
            sem_unlink(serverSemaphoreName.c_str());
            sem_unlink(clientSemaphoreName.c_str());
            return nullptr;
        }
        printInfo("Readed prediction: " + std::to_string(prediction) + "°C", id);
        sem_post(serverSemaphore);

        sem_close(serverSemaphore);
        sem_close(clientSemaphore);
        sem_unlink(serverSemaphoreName.c_str());
        sem_unlink(clientSemaphoreName.c_str());
    } catch (const char *error) {
        perror("error");
        printErr("Error while processing client: " + std::string(error), id);
    }
    return nullptr;
}

void Server::parse(int argc, char *argv[]) {
    static const char *help = "Usage: ./server [--date YEAR MONTH DAY]\n";
    if (argc == 1) {
        srand(time(0));
        date = {rand() % 10 + 2010, rand() % 12, rand() % 28};
    } else if (argc == 5 && !strcmp(argv[1], "--date")) {
        date = {atoi(argv[2]), atoi(argv[3]), atoi(argv[4])};
    } else throw help;
}

void Server::start() {
    printOk("Started server with date " + date.toString());
    fflush(stdout);

    while(true) {
        sleep(1000);
    }
}

int Server::getNextClient() {
    int res;
    pthread_mutex_lock(&nextClientMutex);
        res = nextClient++;
    pthread_mutex_unlock(&nextClientMutex);

    return res;
}

Server::~Server() {
    pthread_mutex_destroy(&nextClientMutex);
}