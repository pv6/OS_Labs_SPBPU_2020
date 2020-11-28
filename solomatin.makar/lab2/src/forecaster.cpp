#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include "forecaster.h"
#include "connection.h"

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
    Forecaster &forecaster = Forecaster::instance();
    forecaster.signalHandled = true;

    int id = info->si_value.sival_int;
    Connection connection(id);

    char *buffer = new char[sizeof(Date)];
    connection.read(buffer, sizeof(Date));
    forecaster.date = Date::deserialize(buffer);
    delete buffer;

    // send signal that result obtained
    kill(forecaster.hostPid, SIGUSR1);
}

Forecaster::Forecaster() : hostPid(-1), signalHandled(false) {
    struct sigaction act;
    act.sa_sigaction = handleSignal;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, nullptr);
}

bool Forecaster::handshake() {
    kill(hostPid, SIGUSR1);

    double beginTime = clock();
    while (!signalHandled) {
        if ((clock() - beginTime) / CLOCKS_PER_SEC > timeout) {
            return false;
        }
        sleep(sleepTime);
    }

    return true;
}

int Forecaster::predict() {
    srand(date.Day + date.Month + date.Year + getpid());

    return -40 + rand() % 80;
}