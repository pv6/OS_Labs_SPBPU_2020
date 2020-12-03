#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "wolf.h"
#include <iostream>


int main () {
    //set follow-fork-mode child
    //set detach-on-fork off

    openlog("host_client", LOG_NOWAIT | LOG_PID, LOG_LOCAL1);
    Wolf* host = Wolf::GetInstance();
    host->Start();
    syslog(LOG_INFO, "Work finish");
    closelog();
    return 0;
}
