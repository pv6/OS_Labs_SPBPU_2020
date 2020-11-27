#include "../include/player_wolf.h"
#include "../include/custom_exception.h"

#include <signal.h>    // signal, SIGUSR1, SIGTERM, SIG_ERR
#include <syslog.h>    // syslog
#include <fcntl.h>     // O_CREAT, O_EXCL
#include <semaphore.h> // sem_t, sem_open, sem_close, SEM_FAILED
#include <errno.h>     // errno
#include <iostream>    // cout
#include <string>      // to_string
#include <unistd.h>    // usleep
#include <iomanip>     // setw

PlayerWolf* PlayerWolf::instance = nullptr;

PlayerWolf::PlayerWolf() :
    Player(Player::PlayerType::PLAYER_TYPE_HOST) {}

PlayerWolf* PlayerWolf::createPlayerWolf() {
    if (signal(SIGUSR1, (sighandler_t)PlayerWolf::handle_signal) == SIG_ERR ||
        signal(SIGTERM, (sighandler_t)PlayerWolf::handle_signal) == SIG_ERR) {
        throw CustomException("signal() error", __FILE__, __LINE__, (int)errno);
    }
    return instance != nullptr ? instance : (instance = new PlayerWolf());
}

void PlayerWolf::handle_signal(int signal) {
    switch(signal) {
    case SIGUSR1:
        syslog(LOG_INFO, "SIGUSR1 signal received");
        if (PlayerWolf::instance != nullptr) {
            PlayerWolf::instance->m_wait_client = false;
        }
        break;
    case SIGTERM:
        syslog(LOG_INFO, "SIGTERM signal received");
        if (PlayerWolf::instance != nullptr) {
            PlayerWolf::instance->m_stop = true;
        }
        break;
    }
}

void PlayerWolf::connect() {
    write_pid();
    syslog(LOG_DEBUG, "attempting to open semaphores");
    sem_unlink(Player::semaphore_name_1.c_str());
    sem_unlink(Player::semaphore_name_2.c_str());
    m_sem1 = sem_open(Player::semaphore_name_1.c_str(), O_CREAT | O_EXCL, 0600, 0);
    m_sem2 = sem_open(Player::semaphore_name_2.c_str(), O_CREAT | O_EXCL, 0600, 0);
    if (m_sem1 == SEM_FAILED ||
        m_sem2 == SEM_FAILED) {
        throw CustomException("sem_open() error", __FILE__, __LINE__, (int)errno);
    }
    syslog(LOG_DEBUG, "attempting to connect");
    m_connection = Conn::createConn(Player::working_dir, sizeof(int));
    m_connection->open();
    syslog(LOG_INFO, "successfully connected");
}

void PlayerWolf::disconnect() {
    syslog(LOG_DEBUG, "attempting to disconnect");
    m_connection->close();
    m_connection->unlink();
    delete m_connection;
    m_connection = nullptr;
    syslog(LOG_INFO, "successfully disconnected");
    syslog(LOG_DEBUG, "attempting to close semaphores");
    if (sem_close(m_sem1) == -1 || 
        sem_close(m_sem2) == -1) {
        throw CustomException("sem_close() error", __FILE__, __LINE__, (int)errno);
    }
    if (sem_unlink(Player::semaphore_name_1.c_str()) == -1 ||
        sem_unlink(Player::semaphore_name_2.c_str()) == -1) {
        throw CustomException("sem_unlink() error", __FILE__, __LINE__, (int)errno);
    }
    clear_pid();
}

void PlayerWolf::play() {
    while (!m_stop) {
        while(!m_stop && m_wait_client);
        if (m_stop) {
            break;
        }
        std::cout << "client arrived\n";
        syslog(LOG_INFO, "game starts");

        int status = game_rules.CLIENT_FLAG_ALIVE;
        int dead_turns = 0;
        int rolled_client;
        while (!m_stop) {
            if (!timed_wait(m_sem2)) { break; }
            int rolled = roll(m_player_stats.ROLL_MIN, m_player_stats.ROLL_MAX);
            m_connection->read((void*)&rolled_client, sizeof(int));
            std::cout 
                << "rolled " << std::setw(3) << rolled
                << "  vs " << std::setw(3) << rolled_client << " client ... ";
            if (status == game_rules.CLIENT_FLAG_DEAD) {
                if (abs(rolled_client - rolled) <= game_rules.REVIVE_DIFF) {
                    status = game_rules.CLIENT_FLAG_ALIVE;
                    dead_turns = 0;
                    std::cout << "revived\n";
                }
                else {
                    dead_turns++;
                    std::cout << "dead for " << dead_turns << " turn(s)\n";
                    if (dead_turns == game_rules.MAX_DEAD_TURNS) {
                        // TODO: send signal
                        break;
                    }
                }
            }
            else {
                if (abs(rolled_client - rolled) > game_rules.MAX_DIFF) {
                    status = game_rules.CLIENT_FLAG_DEAD;
                    std::cout << "died\n"; 
                }
                else {
                    std::cout << "alive\n";
                }
            }

            m_connection->write((void*)&status, sizeof(int));
            post(m_sem1);

            // uncomment if you want to track turns in real-time
            // usleep(250000);
        }
        
        syslog(LOG_INFO, "game ends");
        std::cout << "client kicked out\n";
        m_connection->clear();
        m_wait_client = true;
    }
}

/* better implementation below */
// void PlayerWolf::play() {
//     while (!m_stop) {
//         while(!m_stop && m_wait_client);
//         if (m_stop) {
//             break;
//         }
//         std::cout << "client arrived\n";
//         syslog(LOG_INFO, "game starts");
//         while (!m_stop) {
//             if (!timed_wait(m_sem1)) { break; }
//             int rolled = roll(m_player_stats.ROLL_MIN, m_player_stats.ROLL_MAX);
//             std::cout << "rolled " << rolled << '\n';
//             m_connection->write((void*)&rolled, m_connection->m_max_buff_size);
//             post(m_sem2);
//             // uncomment if you want to track turns in real-time
//             // usleep(250000);
//         }
//         syslog(LOG_INFO, "game ends");
//         std::cout << "client gone\n";
//         m_connection->clear();
//         m_wait_client = true;
//     }
// }
