#include <unistd.h>
#include <syslog.h> // openlog, syslog, closelog
#include <signal.h> // SIGHUP, SIGTERM

#include <string>    // string
#include <exception> // exception

#include "../include/worker09.h"
#include "../include/daemon.h"

int main(int argc, char const* argv[]) {
    openlog("lab1", LOG_NDELAY | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "starting...");

    std::string path_conf = "./conf.conf";
    std::string path_pidf = "/var/run/lab1.pid";
    
    if (argc > 1) {
        path_conf = argv[1];
    }
    if (argc > 2) {
        path_pidf = argv[2];
    }

    Worker* worker = nullptr;
    Daemon* daemon = nullptr;
    try {
        worker = new Worker09(path_conf);
        daemon = Daemon::getDaemon(path_pidf);
        daemon->worker_set(worker);
        if (!daemon->daemonize()) {
            delete daemon;
            delete worker;
            return 0;
        }
        daemon->worker_run();
    }
    catch (std::exception const& exception) {
        syslog(LOG_CRIT, "%s", exception.what());
        syslog(LOG_INFO, "exiting...");
        closelog();
        delete daemon;
        delete worker;
        return -1;
    }

    syslog(LOG_INFO, "exiting...");
    closelog();
    delete daemon;
    delete worker;
    return 0;
}
