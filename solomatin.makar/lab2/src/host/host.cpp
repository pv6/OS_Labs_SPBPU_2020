#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include "date.h"
#include "server.h"
#include "print_utils.h"
#include "connection.h"

int main(int argc, char *argv[]) {
    std::cout << getpid() << std::endl;
    Server &server = Server::instance();
    try {
        server.parse(argc, argv);
        server.start();
    } catch (const char *error) {
        printErr(error);
        perror(error);
    }
    return 0;
}