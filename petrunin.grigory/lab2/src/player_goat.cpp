#include "../include/player_goat.h"
#include "../include/custom_exception.h"

#include <signal.h>    // signal, kill, SIGUSR1
#include <syslog.h>    // syslog
#include <semaphore.h> // sem_open, sem_close, SEM_FAILED
#include <errno.h>     // errno
#include <iostream>    // cout
#include <cmath>       // abs
#include <ctime>       // time_t
#include <iomanip>     // setw

PlayerGoat* PlayerGoat::instance = nullptr;

PlayerGoat::PlayerGoat() :
    Player(Player::PlayerType::PLAYER_TYPE_CLIENT) {}

PlayerGoat* PlayerGoat::createPlayerGoat() {
    if (signal(SIGTERM, (sighandler_t)PlayerGoat::handle_signal) == SIG_ERR) {
        throw CustomException("signal() error", __FILE__, __LINE__, (int)errno);
    }
    return instance != nullptr ? instance : (instance = new PlayerGoat());
}

void PlayerGoat::handle_signal(int signal) {
    switch(signal) {
    case SIGTERM:
        syslog(LOG_INFO, "SIGTERM signal received");
        if (PlayerGoat::instance != nullptr) {
            PlayerGoat::instance->m_stop = true;
        }
        break;
    }
}

void PlayerGoat::connect() {
    write_pid();
    syslog(LOG_DEBUG, "attempting to find host");
    m_host_pid = read_pid(Player::PlayerType::PLAYER_TYPE_HOST);
    if (m_host_pid == 0 || kill(m_host_pid, 0) != 0) {
        throw CustomException("host not found", __FILE__, __LINE__);
    }
    syslog(LOG_DEBUG, "attempting to open semaphores");
    m_sem1 = sem_open(Player::semaphore_name_1.c_str(), 0);
    m_sem2 = sem_open(Player::semaphore_name_2.c_str(), 0);
    if (m_sem1 == SEM_FAILED ||
        m_sem2 == SEM_FAILED) {
        throw CustomException("sem_open() error", __FILE__, __LINE__, (int)errno);
    }
    syslog(LOG_DEBUG, "attempting to connect");
    m_connection = Conn::createConn(Player::working_dir, sizeof(int));
    m_connection->open();
    syslog(LOG_INFO, "successfully connected");
}

void PlayerGoat::disconnect() {
    syslog(LOG_DEBUG, "attempting to disconnect");
    m_connection->close();
    delete m_connection;
    m_connection = nullptr;
    syslog(LOG_INFO, "successfully disconnected");
    syslog(LOG_DEBUG, "attempting to close semaphores");
    if (sem_close(m_sem1) == -1 || 
        sem_close(m_sem2) == -1) {
        throw CustomException("sem_close() error", __FILE__, __LINE__, (int)errno);
    }
    Player::clear_pid();
}

void PlayerGoat::play() {
    // make sure semaphores are at value 0
    timed_wait(m_sem1, (time_t)0);
    timed_wait(m_sem2, (time_t)0);

    // notify host that client has arrived
    if (kill(m_host_pid, SIGUSR1) != 0) {
        throw CustomException("kill() error", __FILE__, __LINE__, (int)errno);
    }

    // start playing
    int status = game_rules.CLIENT_FLAG_ALIVE;
    while (!m_stop) {
        int rolled = (status == game_rules.CLIENT_FLAG_DEAD) ? 
            roll(m_player_stats.ROLL_DEAD_MIN, m_player_stats.ROLL_DEAD_MAX) :
            roll(m_player_stats.ROLL_MIN, m_player_stats.ROLL_MAX);
        std::cout << "rolled " << rolled << '\n';
        m_connection->write((void*)&rolled, sizeof(int));

        post(m_sem2);
        if (!timed_wait(m_sem1)) { break; }
        m_connection->read((void*)&status, sizeof(int));
    }
}

/* better implementation below */
// void PlayerGoat::play() {
//     // make sure semaphores are at value 0
//     timed_wait(m_sem1, (time_t)0);
//     timed_wait(m_sem2, (time_t)0);
//     // notify host that client has arrived
//     if (kill(m_host_pid, SIGUSR1) != 0) {
//         throw CustomException("kill() error", __FILE__, __LINE__, (int)errno);
//     }
//     // start playing
//     bool dead = false;
//     int dead_turns = 0;
//     int rolled_host;
//     do {
//         post(m_sem1);
//         if (!timed_wait(m_sem2)) { break; }
//         int rolled = dead ? 
//             roll(m_player_stats.ROLL_DEAD_MIN, m_player_stats.ROLL_DEAD_MAX) :
//             roll(m_player_stats.ROLL_MIN, m_player_stats.ROLL_MAX);
//         m_connection->read((void*)&rolled_host, m_connection->m_max_buff_size);
//         std::cout 
//             << "rolled " << std::setw(3) << rolled
//             << "  vs " << std::setw(3) << rolled_host << " host ... ";
//         if (dead) {
//             if (abs(rolled_host - rolled) <= game_rules.REVIVE_DIFF) {
//                 dead = false;
//                 dead_turns = 0;
//                 std::cout << "revived\n";
//             }
//             else {
//                 dead_turns++;
//                 std::cout << "dead for " << dead_turns << " turn(s)\n";
//                 if (dead_turns == game_rules.MAX_DEAD_TURNS) break;
//             }
//         }
//         else {
//             if (abs(rolled_host - rolled) > game_rules.MAX_DIFF) {
//                 dead = true;
//                 std::cout << "died\n"; 
//             }
//             else {
//                 std::cout << "alive\n";
//             }
//         }
//     } while (!m_stop);
// }
