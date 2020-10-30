#include <cstdlib>
#include <syslog.h>
#include "Daemon.h"


int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Don't get name of config file";
        return(EXIT_FAILURE);
    }

    openlog ("Daemon_log.log", LOG_PID, LOG_DAEMON);
    try {
        Daemon* daemon = new Daemon(argv[1]);
    } catch (std::runtime_error& re) {
        syslog(LOG_ERR, "%s", re.what());
    }
    Daemon::run();

    syslog (LOG_NOTICE, "Daemon terminated");
    closelog();

    return EXIT_SUCCESS;
}