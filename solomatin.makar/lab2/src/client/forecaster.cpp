#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include "forecaster.h"
#include "global_settings.h"
#include "connection.h"
#include "print_utils.h"

bool Forecaster::parseHostPid(int argc, char *argv[]) {
    static const char *help = "Usage: ./client [--host-pid PID]\n";
    if (argc == 3 && !strcmp(argv[1], "--host-pid")) {
        hostPid = atoi(argv[2]);
        return true;
    } else {
        std::cout << help;
        return false;
    }
}

void Forecaster::handleSignal(int signum, siginfo_t* info, void* ptr) {
    printOk("Received signal from " + std::to_string(info->si_pid));
    Forecaster &forecaster = Forecaster::instance();
    if (forecaster.connection != nullptr) return;

    int id = info->si_value.sival_int;
    printOk("Host returned id: " + std::to_string(id));

    Connection *connection = Connection::connect(id);
    printOk("Connection object created", id);

    printOk("Reading date from server...", id);
    forecaster.readDate();
    printOk("Date read: " + forecaster.date.toString(), id);

    forecaster.sendPrediction();
}

Forecaster::Forecaster() : date{0,0,0} {
    struct sigaction act;
    act.sa_sigaction = handleSignal;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, nullptr);
}

bool Forecaster::handshake() {
    kill(hostPid, SIGUSR1);

    clock_t beginTime = clock();
    while (connection == nullptr) {
        double elapsed = (clock() - beginTime) / CLOCKS_PER_SEC;
        if (elapsed > TIMEOUT) {
            return false;
        }
    }

    return true;
}

void Forecaster::sendPrediction() {
    printOk("Calculating prediction...");
    srand(date.Day + date.Month + date.Year + getpid());
    prediction = -40 + rand() % 80;
    printOk("Writing prediction");
    connection->write((char *)&prediction, sizeof(prediction));
    printOk("Prediction calculated");

    printOk("Trying to release server semaphore");
    sem_post(connection->serverSemaphore);
    printOk("Relased server semaphore!");
}

Forecaster::~Forecaster() {
    if (connection != nullptr) {
        delete connection;
    }
}

void Forecaster::readDate() {
    if (connection == nullptr) return;
    connection->read((char *)&date, sizeof(date));
}