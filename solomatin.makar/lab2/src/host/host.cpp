#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include "date.h"
#include "server.h"
#include "connection.h"

int main(int argc, char *argv[]) {
    Server &server = Server::instance();

    if (!server.parseDate(argc, argv)) return 1;

    try {
        server.start();
    } catch (const char *error) {
        perror(error);
    }

    return 0;
}