//
// Created by aleksandr on 25.11.2020.
//

#include <string>
#include "goat.h"


int main(int argc, char* argv[]){
    openlog("Goat_lab2", LOG_PID | LOG_NDELAY, LOG_USER);

    if (argc != 2)
    {
        syslog(LOG_ERR, "2 arguments expected");
        closelog();
        return 1;
    }

    int pid;
    try
    {
        pid = std::stoi(argv[1]);
    }
    catch (std::exception &e)
    {
        syslog(LOG_ERR, "Invalid pid format");
        closelog();
        return 1;
    }


    Goat& client = Goat::getGoat(pid);
    if (client.openConnection())
    {
        client.run();
    }

    syslog(LOG_INFO, "Goat end");
    closelog();

    return 0;
}