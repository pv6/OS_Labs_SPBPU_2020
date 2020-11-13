#include "../include/player_goat.h" // Player, PlayerGoat

#include <syslog.h>  // openlog, syslog, closelog
#include <exception> // exception

int main(int argc, char const* argv[]) {
    openlog("client", LOG_NDELAY | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "starting...");

    Player* player = nullptr;
    try {
        player = new PlayerGoat();
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
