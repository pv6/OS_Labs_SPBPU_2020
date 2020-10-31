#include <string>
#include <iostream>
#include "daemon.h"

int main(int argc, char **argv) {
    std::string config_file_path;
    openlog("daemon_lab", LOG_NOWAIT | LOG_PID, LOG_DAEMON);
    syslog(LOG_NOTICE, "Log was opened");
    if (argc != 2) {
        syslog(LOG_ERR, "ERROR: nums of args are not valid");
        return EXIT_FAILURE;
    }
    std::string conf_file= argv[1];
    daemon::set_config(conf_file);
    try{
        if (!daemon ::create()){
            syslog(LOG_NOTICE, "Parent process was ended");
            return EXIT_SUCCESS; // end parent process
        }
        daemon::run();
    }
    catch (std::runtime_error const &ex) {
        syslog(LOG_ERR, "Runtime error: %s", ex.what());
        closelog();
        return EXIT_FAILURE;
    }
    closelog();

    return EXIT_SUCCESS;
}