#include <iostream>
#include "daemon.h"
#include <unistd.h>


int main(int argc, char** argv) {
    if (argc != 2) {
        exit(EXIT_FAILURE);
    }
    int pid = (int)fork();
    if (pid == -1) {
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        if (daemon::init(argv[1]) == error::OK) {
            daemon::daemonize();
        } else {
            exit(EXIT_FAILURE);
        }
    }
    return EXIT_SUCCESS;
}
