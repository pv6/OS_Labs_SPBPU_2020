#include "my_host.h"
#include "../core/sys_exception.h"
#include <iostream>
#include <syslog.h>

int main() {
    openlog("lab2_log", LOG_PID, LOG_DAEMON);
    Host* host;
    try {
        host = Host::get();
        host->run();
    } catch (const SysException& ex) {
        syslog(LOG_ERR, ex.what());
        std::cout << "An error occured, see syslog for details\n";
    }
    delete host;
    closelog();
}
