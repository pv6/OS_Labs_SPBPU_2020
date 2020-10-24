#include <unistd.h>
#include <syslog.h> // openlog, syslog, closelog
#include <signal.h> // SIGHUP, SIGTERM

#include <string>    // string
#include <exception> // exception

#include "../include/daemon09.h"

static Daemon* g_daemon = nullptr;

void handle_signal(int signal) {
    switch(signal) {
    case SIGHUP:
        syslog(LOG_INFO, "SIGHUP signal catched");
        if (g_daemon != nullptr) {
            g_daemon->reconfigure();
        }
        break;
    case SIGTERM:
        syslog(LOG_INFO, "SIGTERM signal catched");
        closelog();
        g_daemon->stop();
    }
}

int main(int argc, char const* argv[]) {
    openlog("lab1", LOG_NDELAY | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "starting...");

    std::string default_config_path = "./conf.conf";
    std::string default_pid_path = "/var/run/lab1.pid";
    std::string actual_config_path = default_config_path;
    std::string actual_pid_path = default_pid_path;
    
    if (argc > 1) {
        actual_config_path = argv[1];
    }
    if (argc > 2) {
        actual_pid_path = argv[2];
    }

    try {
        g_daemon = new Daemon09(actual_config_path, actual_pid_path);
        if (!g_daemon->daemonize(handle_signal)) {
            return 0;
        }
        g_daemon->work();
    }
    catch (std::exception const& exception) {
        syslog(LOG_CRIT, "%s", exception.what());
        syslog(LOG_INFO, "exiting...");
        closelog();
        return -1;
    }

    syslog(LOG_INFO, "exiting...");
    closelog();
    return 0;
}
