#ifndef VIA8_PLAYER_GOAT_H
#define VIA8_PLAYER_GOAT_H

#include "player.h"

#include <string> // string

// classic implementation of singleton
class PlayerGoat final : public Player {
private:
    static PlayerGoat* m_instance;

public:
    static PlayerGoat* getPlayerGoat();
    static void handle_signal(int signal);
    void connect() override;
    void disconnect() override;
    void play() override;

private:
    PlayerGoat();
    PlayerGoat(PlayerGoat const&) = delete;
    PlayerGoat& operator=(PlayerGoat const&) = delete;
};

#endif /* VIA8_PLAYER_GOAT_H */
