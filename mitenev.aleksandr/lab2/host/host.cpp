//
// Created by aleksandr on 26.11.2020.
//

#include "wolf.h"

int main(){
    openlog("Wolf_lab2", LOG_PID | LOG_NDELAY, LOG_USER);
    Wolf& wolf = Wolf::getWolf();

    if(wolf.openConnection()) {
        wolf.run();
    }
    syslog(LOG_INFO, "Wolf end");
    closelog();
    return 0;
}