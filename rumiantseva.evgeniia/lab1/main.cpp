#include "Daemon.h"

int main(int argc, char *argv[]) {
    openlog("Daemon_FileReplace", LOG_PID, LOG_DAEMON);
    if (argc < 2) {
        syslog(LOG_ERR, "Configurations file is required as a second argument");
        closelog();
        return 1;
    }
    try {
        Daemon& daemon = Daemon::GetDaemonInst();
        std::string configPath = argv[1];
        daemon.SetConfig(configPath);
        daemon.Run();
    } catch (CustomException& e) {
        if (e.getErrorNumber()) {
            syslog(LOG_ERR, "%s", e.what());
            closelog();
            return 1;
        }
        closelog();
        return 0;
    }
    closelog();
    return 0;
}
