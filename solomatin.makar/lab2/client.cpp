#include <iostream>
#include <string.h>
#include "date.h"
#include "server.h"

int main(int argc, char *argv[]) {
    Date date;
    if (argc == 1) {
        date = {rand() % 2020, rand() % 12, rand() % 28};
    } else if (argc == 5 && !strcmp(argv[1], "--date")) {
        date = {atoi(argv[2]), atoi(argv[3]), atoi(argv[4])};
    }

    Server *server = Server::instance();
    server->start(date);

    return 0;
}