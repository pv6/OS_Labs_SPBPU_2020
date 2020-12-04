#include <string.h>
#include <stdio.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include "forecaster.h"
#include "global_settings.h"
#include "connection.h"
#include "print_utils.h"

void Forecaster::parse(int argc, char *argv[]) {
    static const char *help = "Usage: ./client (PID | --host-pid PID)\n";
    if (argc == 3 && !strcmp(argv[1], "--host-pid")) {
        hostPid = atoi(argv[2]);
    } else if (argc == 2) {
        hostPid = atoi(argv[1]);
    } else throw help;
}

void Forecaster::handleSignal(int signum, siginfo_t* info, void* ptr) {
    printOk("Received signal from " + std::to_string(info->si_pid));
    Forecaster &forecaster = Forecaster::instance();
    if (forecaster.connection != nullptr) return;

    int id = forecaster.id = info->si_value.sival_int;
    printOk("Host returned id: " + std::to_string(id));

    try {
        forecaster.connection = new Connection(id);
    } catch (const char *error) {
        perror(error);
        printErr("Connection object creation failed", id);
        forecaster.connection = nullptr;

        return;
    }
    printOk("Connection object created", id);
}

Forecaster::Forecaster() {
    struct sigaction act;
    act.sa_sigaction = handleSignal;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, nullptr);

    id = hostPid = -1;
}

void Forecaster::handshake() {
    kill(hostPid, SIGUSR1);

    clock_t beginTime = clock();
    while (connection == nullptr) {
        double elapsed = (clock() - beginTime) / CLOCKS_PER_SEC;
        if (elapsed > TIMEOUT) {
            throw "Could not establish connection to host";
        }
    }
}

Forecaster::~Forecaster() {
    if (connection != nullptr) delete connection;
}

void Forecaster::forecast() {
    if (connection == nullptr) {
        return;
    }
    std::string clientSemName = CLIENT_SEMAPHORE + std::to_string(id);
    std::string serverSemName = SERVER_SEMAPHORE + std::to_string(id);
    sem_t *clientSem = sem_open(clientSemName.c_str(), O_RDWR);
    sem_t *serverSem = sem_open(serverSemName.c_str(), O_RDWR);

    if (clientSem == SEM_FAILED || serverSem == SEM_FAILED) {
        throw "Could not open semaphore";
    }
    printOk("Semaphores opened", id);

    Date date;
    printOk("Wating when server release client semaphore...", id);
    sem_wait(clientSem);
    printOk("Server released client semaphore!", id);
    if (!connection->read((char *)&date, sizeof(date))) {
        throw "Could not read date";
    }
    printInfo("Date read: " + date.toString(), id);

    srand(getpid() + date.Day + date.Month + date.Year);
    int prediction = -40 + rand() % 80;
    if (!connection->write((char *)&prediction, sizeof(prediction))) {
        throw "Could not write prediction";
    };
    printInfo("Writed prediction: " + std::to_string(prediction) + "°C", id);
    sem_post(clientSem);
    sem_post(serverSem);

    sem_close(clientSem);
    sem_close(serverSem);
    printOk("Exiting successfully", id);
}