#include "../include/player_wolf.h" // Player, PlayerWolf

#include <syslog.h>  // openlog, syslog, closelog
#include <exception> // exception

int main(int argc, char const* argv[]) {
    openlog("host", LOG_NDELAY | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "starting...");

    Player* player = nullptr;
    try {
        player = new PlayerWolf();
        player->connect();
        player->play();
    }
    catch (std::exception const& exception) {
        delete player;
        syslog(LOG_CRIT, "%s", exception.what());
        syslog(LOG_INFO, "exiting...");
        closelog();
        return -1;
    }

    delete player;
    syslog(LOG_INFO, "exiting...");
    closelog();
    return 0;
}
