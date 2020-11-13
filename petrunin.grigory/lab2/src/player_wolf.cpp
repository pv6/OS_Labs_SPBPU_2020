#include "../include/player_wolf.h"
#include "../custom_exception.h"

#include <signal.h> // SIGUSR1
#include <syslog.h> // syslog
#include <unistd.h> // usleep

PlayerWolf* PlayerWolf::m_instance = nullptr;

PlayerWolf* PlayerWolf::getPlayerWolf() {
    return m_instance != nullptr ? m_instance : (m_instance = new PlayerWolf());
}

void PlayerWolf::handle_signal(int signal) {
    switch(signal) {
    case SIGUSR1:
        syslog(LOG_INFO, "SIGUSR1 signal received");
        if (PlayerWolf::m_instance != nullptr) {
            // TODO
        }
    }
}

void PlayerWolf::connect() {
    Player::write_pid(Player::PlayerType::PLAYER_TYPE_HOST);
    usleep(1000000 * 20);
}
