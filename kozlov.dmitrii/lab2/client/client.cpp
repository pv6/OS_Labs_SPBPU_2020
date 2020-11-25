#include <iostream>
#include <syslog.h>
#include <unistd.h>

#include "goat.h"

int main (int argc, char* argv[]) {
    openlog("GameClient", LOG_PID, LOG_DAEMON);
    syslog(LOG_INFO, "Client's pid is: %d", getpid());
    std::cout << "Client's pid is: " << getpid() << std::endl;

    if (argc != 2) {
        syslog(LOG_ERR, "Expected cmd argument: PID");
        std::cout << "Expected cmd argument: PID" << std::endl;
        return -1;
    }

    uint32_t hostPid = atoi(argv[1]);

    syslog(LOG_ERR, "Client is working with host (pid): %d", hostPid);
    std::cout << "Client is working with host (pid): " << hostPid << std::endl;

    try {
        Goat& goat = Goat::GetInstance(hostPid);
        goat.PrepareGame();
        goat.StartGame();
    } catch (std::runtime_error &e) {
        syslog(LOG_ERR, "%s", e.what());
        std::cout << e.what() << std::endl;
        closelog();
        return 1;
    }
    closelog();
    return 0;
}
