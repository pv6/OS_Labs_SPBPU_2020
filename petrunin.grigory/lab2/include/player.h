#ifndef VIA8_PLAYER_H
#define VIA8_PLAYER_H

#include "conn.h"

#include <string>      // string
#include <ctime>       // time_t
#include <semaphore.h> // sem_t

class Player {
private:
    static std::string const pid_path_host;
    static std::string const pid_path_client;
    static std::string const& get_pid_path(int player_type);

    void check_working_dir() const;

    int const m_player_type;
    
protected:
    enum PlayerType {
        PLAYER_TYPE_HOST = 0,
        PLAYER_TYPE_CLIENT
    };

    Conn* m_connection = nullptr;
    sem_t* m_sem1 = nullptr;
    sem_t* m_sem2 = nullptr;
    bool m_stop = false;

    static std::string const semaphore_name_1;
    static std::string const semaphore_name_2;
    static std::string const working_dir;
    struct {
        int const MAX_DIFF = 70;
        int const REVIVE_DIFF = 20;
        int const MAX_DEAD_TURNS = 2;
        int const CLIENT_FLAG_DEAD = 0;
        int const CLIENT_FLAG_ALIVE = 1;
    } game_rules;

    // connection
    int read_pid(int player_type) const;
    void write_pid() const;
    void clear_pid() const;

    // synchronization
    void post(sem_t* semaphore) const;
    void wait(sem_t* semaphore) const;
    bool timed_wait(sem_t* semaphore, time_t time_slice = (time_t)3) const;
    
    // game logic
    int roll(int min, int max) const;

public:
    Player(int player_type);
    virtual ~Player() = 0;
    virtual void connect() = 0;
    virtual void disconnect() = 0;
    virtual void play() = 0;
};

#endif /* VIA8_PLAYER_H */
