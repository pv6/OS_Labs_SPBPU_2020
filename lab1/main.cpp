#include "daemon.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Error: exprected 2 arguments, got %d \n", argc);
        return EXIT_FAILURE;
    }

    openlog ("lab1_daemons", LOG_PID, LOG_DAEMON);
    syslog (LOG_NOTICE, "Open syslog.");

    file_handler* fh = file_handler::getInstance(std::string(argv[1]));
    if (fh == nullptr){
        syslog (LOG_ERR, "File handler isn't created.");
        return EXIT_FAILURE;
    }

    int last_error = fh->parse_config();
    if (last_error != EXIT_SUCCESS){
        syslog (LOG_ERR, "Config file isn't parsed.");
        return last_error;
    }
    
    fflush(stdout);

    daemon_parent* new_daemon = daemon_parent::getInstance(fh);
    if (new_daemon == nullptr){
        syslog (LOG_ERR, "Daemon instance isn't create.");
        return EXIT_FAILURE;
    }
    try{
        new_daemon->create();
        while (true) {
            new_daemon->run_process(fh->getFolder().c_str());
            sleep (fh->getInterval());
        }
    } catch(int e){
        if (e == EXIT_FAILURE)
            return EXIT_FAILURE;
        if (e == EXIT_SUCCESS)
            return EXIT_SUCCESS;
    }

    syslog (LOG_NOTICE, "Close syslog.");
    closelog();

    delete new_daemon;

    return EXIT_SUCCESS;
}
