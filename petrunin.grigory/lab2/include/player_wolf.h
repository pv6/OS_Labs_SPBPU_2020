#ifndef VIA8_PLAYER_WOLF_H
#define VIA8_PLAYER_WOLF_H

#include "player.h"

#include <string> // string

// classic implementation of singleton
class PlayerWolf final : public Player {
private:
    static PlayerWolf* m_instance;

public:
    static PlayerWolf* getPlayerWolf();
    static void handle_signal(int signal);
    void connect() override;
    void disconnect() override;
    void play() override;

private:
    PlayerWolf();
    PlayerWolf(PlayerWolf const&) = delete;
    PlayerWolf& operator=(PlayerWolf const&) = delete;
};

#endif /* VIA8_PLAYER_WOLF_H */
