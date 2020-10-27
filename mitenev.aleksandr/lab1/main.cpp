#include <iostream>
#include "daemon.h"


int main(int argc, char **argv) {
    if (argc != 2)
    {
        std::cout << "Not enough arguments" << std::endl;
        return 1;
    }

    if (Daemon::init(argv[1])){
        Daemon::run();
        Daemon::terminate();
    }

    return 0;
}
