#include <iostream>
#include "daemon.h"
#include <unistd.h>
#include "user_exception.h"


int main(int argc, char** argv) {
    if (argc != 2) {
        return -1;
    }
    pid_t pid = fork();
    if (pid == -1) {
        return -1;
    } else if (pid == 0) {
        if (daemon::init(argv[1]) == error::OK) {
            try {
                daemon::daemonize();
            }
            catch (user_exception& e) {
                if (e.is_exit_error())
                    return -1;
                else
                    return 0;
            }
        } else {
            return -1;
        }
    }
    return 0;
}
