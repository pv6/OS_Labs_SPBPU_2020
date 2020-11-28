#include <iostream>
#include <unistd.h>
#include <string.h>
#include "date.h"
#include "server.h"

int *parseArgs(int argc, char *argv[]) {
    static const char *help = "Usage: ./client [--host-pid PID]\n";
    if (argc == 3 && !strcmp(argv[1], "--host-pid")) {
        return new int(atoi(argv[2]));
    } else {
        printf(help);
        return nullptr;
    }
} 

bool handshake(pid_t hostPid) {
    kill(hostPid, SIGUSR1);
    
    // synchronous waiting for response with timeout
    while (!signalHandled) {
        sleep(1);

        if (sleepedTime > TIMEOUT) {
            return false;
        }
    }

    return true;
}

int main(int argc, char *argv[]) {
    pid_t *hostPid = parseArgs(argc, argv);
    if (hostPid == nullptr) return 1;

    delete hostPid;
    return 0;
}