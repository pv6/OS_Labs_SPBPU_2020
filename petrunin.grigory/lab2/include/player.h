#ifndef VIA8_PLAYER_H
#define VIA8_PLAYER_H

#include <string>

class Player {
private:
    static std::string const pid_path_host;
    static std::string const pid_path_client;
    
protected:
    enum PlayerType {
        PLAYER_TYPE_HOST = 0,
        PLAYER_TYPE_CLIENT
    }

    void write_pid(Player::PlayerType type);
    void clear_pid(Player::PlayerType type);

public:
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void play() = 0;
};

#endif /* VIA8_PLAYER_H */
