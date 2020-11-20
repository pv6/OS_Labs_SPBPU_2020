#ifndef VIA8_PLAYER_WOLF_H
#define VIA8_PLAYER_WOLF_H

#include "player.h"

#include <string>      // string
#include <semaphore.h> // sem_t

// classic implementation of singleton
class PlayerWolf final : public Player {
private:
    static PlayerWolf* instance;

    bool m_wait_client = true;
    struct {
        int const roll_min = 1;
        int const roll_max = 100;
    } m_player_stats;

    PlayerWolf();
    PlayerWolf(PlayerWolf const&) = delete;
    PlayerWolf& operator=(PlayerWolf const&) = delete;

public:
    static PlayerWolf* createPlayerWolf();
    static void handle_signal(int signal);
    ~PlayerWolf() override {};
    void connect() override;
    void disconnect() override;
    void play() override;
};

#endif /* VIA8_PLAYER_WOLF_H */
