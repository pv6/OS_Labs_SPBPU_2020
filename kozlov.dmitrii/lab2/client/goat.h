#pragma once
#include <semaphore.h>
#include <string>

#include "../connections/conn.h"
#include "../host/game.h"

class Goat {
public:
    ~Goat();

    static Goat& GetInstance(int hostPid = 0) noexcept(false);

    void PrepareGame() noexcept;
    void StartGame() noexcept;

private:
    Goat(int hostPid) noexcept(false);

    Goat(Goat& w) = delete;
    Goat& operator=(const Goat& w) = delete;

    int GenerateValue() noexcept;
    bool GenAndWriteValue() noexcept;

    int IsAlive() noexcept { return _goatState == GOAT_STATE::ALIVE; }

    bool SemWait(sem_t* sem) noexcept;
    bool SemSignal(sem_t* sem) noexcept;
    void ConnectToSem(sem_t** sem, std::string semName) noexcept;

    static void OnSignalRecieve(int sig) noexcept;

    Conn _conn;
    sem_t* _semHost;
    sem_t* _semClient;
    uint32_t _hostPid;

    GOAT_STATE _goatState = GOAT_STATE::ALIVE;
};
