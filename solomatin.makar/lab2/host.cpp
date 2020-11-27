#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include "date.h"
#include "server.h"
#include "connection.h"

Date *parseArgs(int argc, char *argv[]) {
    static const char *help = "Usage: ./server [--date YEAR MONTH DAY]\n";
    if (argc == 1) {
        return new Date{rand() % 10 + 2010, rand() % 12, rand() % 28};
    } else if (argc == 5 && !strcmp(argv[1], "--date")) {
        return new Date{atoi(argv[2]), atoi(argv[3]), atoi(argv[4])};
    } else {
        printf(help);
        return nullptr;
    }
}

void handleSignal(int signum, siginfo_t* info, void* ptr) {
    std::cout << "---" << std::endl;
    std::cout << "---" << std::endl << "Received SIGUSR1 from " << info->si_pid << std::endl;

    // create thread 
    // and in this thread, create new connection
    // then, send signal with fid of socket/pipe/mq
    // and compute wheather in day,
    // and with this connection send this wheather through this fid
    // then close connection
}

int main(int argc, char *argv[]) {
    Date *date = parseArgs(argc, argv);
    if (date == nullptr) return 1;

    std::cout << "Started server with date " << date->toString() << std::endl;
    fflush(stdout);

    struct sigaction act;
    act.sa_sigaction = handleSignal;
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, nullptr);

    while (true) pause();

    delete date;
    return 0;
}