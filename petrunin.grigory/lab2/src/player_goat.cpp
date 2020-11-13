#include "../include/player_goat.h"
#include "../custom_exception.h"

#include <signal.h> // SIGUSR1
#include <syslog.h> // syslog
#include <unistd.h> // usleep

PlayerGoat* PlayerGoat::m_instance = nullptr;

PlayerGoat* PlayerGoat::getPlayerGoat() {
    return m_instance != nullptr ? m_instance : (m_instance = new PlayerGoat());
}

void PlayerGoat::handle_signal(int signal) {
    switch(signal) {
    case SIGUSR1:
        syslog(LOG_INFO, "SIGUSR1 signal received");
        if (PlayerGoat::m_instance != nullptr) {
            // TODO
        }
    }
}

void PlayerGoat::connect() {
    Player::write_pid(Player::PlayerType::PLAYER_TYPE_CLIENT);
    usleep(1000000 * 20);
}
