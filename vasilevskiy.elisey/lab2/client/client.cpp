#include <syslog.h>
#include "client.h"
#include <stdexcept>

int main(int argc, char *argv[]) {
    openlog("CLIENT-LAB2", LOG_PID | LOG_NDELAY, LOG_USER);
    if (argc != 2) {
        syslog(LOG_ERR, "Not enough arguments");
        closelog();
        return EXIT_FAILURE;
    }
    int pid;
    try {
        pid = std::stoi(argv[1]);
    }
    catch (std::exception &e) {
        syslog(LOG_ERR, "Wrong format of pid");
        closelog();
        return EXIT_FAILURE;
    }
    Client &client = Client::getInstance(pid);
    try {
        client.openConnection();
    } catch (std::runtime_error &error) {
        syslog(LOG_ERR, "%s", error.what());
        closelog();
        return EXIT_FAILURE;
    }
    try {
        client.start();
    } catch (std::runtime_error &error) {
        syslog(LOG_ERR, "%s", error.what());
        client.terminate();
        closelog();
        return EXIT_FAILURE;
    } catch (...) {
        syslog(LOG_ERR, "Undefined exception");
        client.terminate();
        closelog();
        return EXIT_FAILURE;
    }
    client.terminate();
    closelog();
    return EXIT_SUCCESS;
}
