#include "host_cls.h"

int main(int argc, char* argv[]){
    openlog("HOST-LAB2", LOG_PID | LOG_NDELAY, LOG_USER);
    Host &host = Host::getInstance();
    if (argc != 3){
        syslog(LOG_ERR, "give only input and output files");
        std::cout << "Unexpected args" << std::endl;
        return EXIT_FAILURE;
    }
    
    try {
        std::cout << "open connection" << std::endl;
        host.openConn();
    } catch (std::runtime_error &error) {
        syslog(LOG_ERR, "%s", error.what());
        closelog();
        return EXIT_FAILURE;
    }
    try {
        std::cout << "start host process" << std::endl;
        host.startHost(argv[1], argv[2]);
    } catch (std::runtime_error &error) {
        syslog(LOG_ERR, "%s", error.what());
        host.termHost();
        closelog();
        return EXIT_FAILURE;
    } catch (...) {
        syslog(LOG_ERR, "Undefined exception");
        host.termHost();
        closelog();
        return EXIT_FAILURE;
    }
    host.termHost();
    closelog();
    return EXIT_SUCCESS;
}
