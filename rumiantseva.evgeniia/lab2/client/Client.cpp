//
// Created by Evgenia on 08.11.2020.
//

#include "Goat.h"

int main(int argc, char* argv[]) {
    openlog("Goat_Client", LOG_PID, LOG_DAEMON);
    if (argc < 2) {
        syslog(LOG_ERR, "2nd arg is required");
        closelog();
        return 1;
    }

    int pid;
    try {
        pid = std::stoi(argv[1]);
    }
    catch (std::exception &e) {
        syslog(LOG_ERR, "smth wrong with passed pid");
        closelog();
        return 1;
    }

    Goat& goat = Goat::GetGoatInst();
    goat.SetHostPid(pid);
    if (goat.SetupConnection()) {
        syslog(LOG_NOTICE, "Goat started");
        goat.Start();
    }
    syslog(LOG_NOTICE, "Goat is finished");
    closelog();
    return 0;
}
