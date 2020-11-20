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
        int const roll_min = 1;
        int const roll_max = 100;
        int const roll_dead_min = 1;
        int const roll_dead_max = 50;
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
