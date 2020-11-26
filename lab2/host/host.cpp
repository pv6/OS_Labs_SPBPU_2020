//
// Created by Daria on 11/26/2020.
//


#include <syslog.h>
#include <unistd.h>
#include "server.h"
#include <iostream>


int main(int argc, char**argv) {
    openlog("host_client", LOG_NOWAIT | LOG_PID, LOG_LOCAL1);
    if (argc != 2) {
        syslog(LOG_ERR, "Number of clients should be the second arg");
        closelog();
        return 1;
    }
    int n = std::atoi(argv[1]);
    if (n <= 0) {
        syslog(LOG_ERR, "Number of clients should be positive");
        closelog();
        return 1;
    }
    server *server = server::get_instance();
    syslog(LOG_INFO, "Host started with pid %d.", getpid());
    server->set_num_of_clients(n);
    syslog(LOG_INFO, "Number of clients is set to %d", n);
    server->start();
    syslog(LOG_INFO, "host: stopped.");
    closelog();

    return 0;
}
