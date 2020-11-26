#pragma once
#include <semaphore.h>
#include <signal.h>
#include <string.h>
#include <string>

#include "../connections/conn.h"
#include "game.h"

class Wolf {
public:
    ~Wolf();

    static Wolf& GetInstance(int host_pid) noexcept(false);

    void StartGame() noexcept;
    void PrepareGame() noexcept;

private:
    Wolf(int host_pid) noexcept(false);

    Wolf(Wolf& w) = delete;
    Wolf& operator=(const Wolf& w) = delete;

    int GetValue() noexcept;
    bool SemWait(sem_t* sem) noexcept;
    bool SemSignal(sem_t* sem) noexcept;

    static void HandleSignal(int sig, siginfo_t* info, void* ptr) noexcept;
private:
    Conn _conn;
    sem_t* _pSemHost;
    sem_t* _pSemClient;

    GOAT_STATE _goatState = GOAT_STATE::ALIVE;

    bool _isClientConnected;
    int _clientPid;
    int _hostPid;

    std::string _semHostName;
    std::string _semClientName;
};
