#include <iostream>
#include "daemon.h"
#include <unistd.h>
#include "user_exception.h"


int main(int argc, char** argv) {
    if (argc != 2) {
        return -1;
    }
    try {
        error::error_name status = daemon::init(std::string(argv[1]));
        if (status == error::OK) {
            daemon::daemonize();
        }
    }
    catch (user_exception& e) {
        if (e.is_exit_error())
            return -1;
        else
            return 0;
    }

    return 0;
}
