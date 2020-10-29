#include <syslog.h>
#include <stdexcept>
#include "Daemon.h"

int main(int argc, char *argv[]) {
    openlog("DAEMON-LAB1", LOG_PID | LOG_NDELAY, LOG_USER);
    if (argc != 2) {
        syslog(LOG_ERR, "Not enough arguments");
        return EXIT_FAILURE;
    }
    try {
        if (Daemon::init(argv[1])) {
            Daemon::run();
            closelog();
        }
    } catch (std::runtime_error &error) {
        syslog(LOG_ERR, "%s", error.what());
        closelog();
        return EXIT_FAILURE;
    } catch (...) {
        syslog(LOG_ERR, "Undefined exception");
        closelog();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
