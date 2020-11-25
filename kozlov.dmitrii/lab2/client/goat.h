#pragma once
#include <semaphore.h>
#include <string>

#include "../connections/conn.h"
#include "../host/game.h"

class Goat {
public:
    ~Goat();

    static Goat& GetInstance(int hostPid = 0);

    void PrepareGame();
    void StartGame();

private:
    Goat(int hostPid);

    Goat(Goat& w) = delete;
    Goat& operator=(const Goat& w) = delete;

    int GenerateValue();
    bool GenAndWriteValue();

    int IsAlive(){ return _goatState == GOAT_STATE::ALIVE; }

    bool SemWait(sem_t* sem);
    bool SemSignal(sem_t* sem);
    void ConnectToSem(sem_t** sem, std::string semName);

    static void OnSignalRecieve(int sig);

    Conn _conn;
    sem_t* _semHost;
    sem_t* _semClient;
    uint32_t _hostPid;

    GOAT_STATE _goatState = GOAT_STATE::ALIVE;
};
