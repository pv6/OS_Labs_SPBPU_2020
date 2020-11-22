//
// Created by Evgenia on 08.11.2020.
//

#include "Wolf.h"


int main(int argc, char**argv) {
    openlog("Wolf_Host", LOG_PID, LOG_DAEMON);

    if (argc != 2) {
        syslog(LOG_ERR, "Number of goats should be passed as 2nd arg");
        closelog();
        return 1;
    }
    int n;
    try {
        n = std::atoi(argv[1]);
    } catch (std::exception &e) {
        syslog(LOG_ERR, "smth wrong with passed n");
        return 1;
    }
    if (n <= 0) {
        syslog(LOG_ERR, "Number of goats should be positive");
        closelog();
        return 1;
    }

    Wolf& wolf = Wolf::GetWolfInst();
    wolf.SetGoatsNumber(n);

    if (wolf.SignalToClients()) {
        syslog(LOG_NOTICE, "Starting Wolf");
        wolf.Start();
    }
    syslog(LOG_NOTICE, "Wolf finished");
    closelog();
    return 0;
}
