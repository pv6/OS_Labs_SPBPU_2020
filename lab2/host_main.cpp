#include "host_cls.h"
#include <syslog.h>
#include <iostream>
#include <stdexcept>
#include <unistd.h>

int main(int argc, char* argv[]){
    openlog("HOST-LAB2", LOG_PID | LOG_NDELAY, LOG_USER);
    Host &host = Host::getInstance();
    if (argc != 3)
        syslog(LOG_ERR, "give only input and output files");
    try {
        host.openConn();
    } catch (std::runtime_error &error) {
        syslog(LOG_ERR, "%s", error.what());
        closelog();
        return EXIT_FAILURE;
    }
    try {
        host.startHost(argv[1], argv[2]);
    } catch (std::runtime_error &error) {
        syslog(LOG_ERR, "%s", error.what());
        host.termConn();
        closelog();
        return EXIT_FAILURE;
    } catch (...) {
        syslog(LOG_ERR, "Undefined exception");
        host.termConn();
        closelog();
        return EXIT_FAILURE;
    }
    host.termConn();
    closelog();
    return EXIT_SUCCESS;
}
