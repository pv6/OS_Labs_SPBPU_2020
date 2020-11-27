#include "../include/player_goat.h" // Player, PlayerGoat
#include "../include/custom_exception.h" // CustomException

#include <syslog.h>  // openlog, syslog, closelog
#include <exception> // exception
#include <iostream>  // cout
#include <cstring>   // strlen

int main(int argc, char const* argv[]) {
    openlog("client", LOG_NDELAY | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "starting...");

    int return_code = 0;
    Player* player = nullptr;
    try {
        player = PlayerGoat::createPlayerGoat();
        player->connect();
        player->play();
        player->disconnect();
    }
    catch (CustomException const& exception) {
        std::cout << 
            exception.what() << '\n' << 
            (strlen(exception.errn()) ? exception.errn() : "") << '\n' <<
            exception.file() << '\n' << 
            exception.line() << '\n';
        syslog(LOG_CRIT, "%s", exception.what());
        return_code = 1;
    }
    catch (std::exception const& exception) {
        std::cout << exception.what() << '\n';
        syslog(LOG_CRIT, "%s", exception.what());
        return_code = 2;
    } 

    delete player;
    syslog(LOG_INFO, "exiting...");
    closelog();
    return return_code;
}
