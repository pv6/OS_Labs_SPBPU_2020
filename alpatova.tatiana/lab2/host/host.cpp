#include "my_host.h"
#include <syslog.h>
#include <iostream>
#include <unistd.h>

int main(){
    openlog("lab2", LOG_NOWAIT | LOG_PID | LOG_PERROR, LOG_LOCAL1);
    syslog(LOG_NOTICE, "host: started with pid %d.", getpid());
    my_host & host = my_host::get_instance();
    if (host.open_connection())
        host.run();
    else
    {
        syslog(LOG_NOTICE, "host: can not open conn.");
        closelog();
    }

//    return EXIT_SUCCESS;
}