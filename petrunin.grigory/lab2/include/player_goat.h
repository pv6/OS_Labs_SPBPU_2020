#ifndef VIA8_PLAYER_GOAT_H
#define VIA8_PLAYER_GOAT_H

#include "player.h"

#include <string>      // string
#include <semaphore.h> // sem_t

// classic implementation of singleton
class PlayerGoat final : public Player {
private:
    static PlayerGoat* instance;

    int m_host_pid = 0;
    struct {
        int const ROLL_MIN = 1;
        int const ROLL_MAX = 100;
        int const ROLL_DEAD_MIN = 1;
        int const ROLL_DEAD_MAX = 50;
    } m_player_stats;

    PlayerGoat();
    PlayerGoat(PlayerGoat const&) = delete;
    PlayerGoat& operator=(PlayerGoat const&) = delete;

public:
    static PlayerGoat* createPlayerGoat();
    static void handle_signal(int signal);
    void connect() override;
    void disconnect() override;
    void play() override;
    ~PlayerGoat() override {};
};

#endif /* VIA8_PLAYER_GOAT_H */
