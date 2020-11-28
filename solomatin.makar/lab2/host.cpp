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
        srand(time(0));
        return new Date{rand() % 10 + 2010, rand() % 12, rand() % 28};
    } else if (argc == 5 && !strcmp(argv[1], "--date")) {
        return new Date{atoi(argv[2]), atoi(argv[3]), atoi(argv[4])};
    } else {
        printf(help);
        return nullptr;
    }
} 

void handleSignal(int signum, siginfo_t* info, void* ptr) {
    int pid = info->si_pid;
    std::cout << "---" << std::endl;
    std::cout << "---" << std::endl << "Received SIGUSR1 from " << pid << std::endl;

    // create thread 
    // and in this thread, create new connection
    // then, send signal with fid of socket/pipe/mq
    // and compute wheather in day,
    // and with this connection send this wheather through this fid
    // then close connection

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_create(&tid, &attr, newConnection, nullptr);
}

void *newConnection(void *pid) {
    Connection connection;

    // notify client that channel created 
    union sigval sv; 
    sv.sival_int = connection.getFd();
    sigqueue(*(int *)pid, SIGUSR1, sv);

    connection.write(date);

    // wait (with timeout) client's response in channel
    connection.read(??);
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