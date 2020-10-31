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
        if (!Daemon::init(argv[1]))
            return 0;
        Daemon::run();
    } catch (std::runtime_error& re) {
        syslog(LOG_ERR, "%s", re.what());
    }

    syslog (LOG_NOTICE, "Daemon terminated");
    closelog();

    return EXIT_SUCCESS;
}