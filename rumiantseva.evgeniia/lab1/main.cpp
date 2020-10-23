#include "Daemon.h"

void signal_handler(int signal) {
    switch (signal) {
        case SIGHUP:
            syslog(LOG_NOTICE, "Updating configurations...");
            Daemon::GetDaemonInst().ReadConfig();
            break;
        case SIGTERM:
            syslog(LOG_NOTICE, "Daemon is terminated");
            // removing pid-file
            unlink(Daemon::pid_path.c_str());
            closelog();
            exit(EXIT_SUCCESS);
        default:
            syslog(LOG_NOTICE, "Signal %i is not handled", signal);
    }
}

int main(int argc, char *argv[]) {
    openlog("Daemon_FileReplace", LOG_PID, LOG_DAEMON);
    if (argc < 2) {
        syslog(LOG_ERR, "Configurations file is required as a second argument");
        closelog();
        exit(EXIT_FAILURE);
    }
    Daemon& daemon = Daemon::GetDaemonInst();
    daemon.SetHandler(signal_handler);
    daemon.SetConfig(argv[1]);
    daemon.Run();
}
