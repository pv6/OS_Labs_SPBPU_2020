#include <iostream>
#include <cstring>
#include <random>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>

#include "goat.h"

Goat::Goat(int hostPid)
    : _conn(hostPid, false) {

    openlog("GameClient", LOG_PID, LOG_DAEMON);

    std::string semHostName = std::string("host_" + std::to_string(_hostPid));
    std::string semClientName = std::string("client_" + std::to_string(_hostPid));

    _hostPid = hostPid;

    ConnectToSem(&_semHost, semHostName);
    if (_semHost == SEM_FAILED) {
        throw std::runtime_error("sem_host wasn't opened");
    }

    ConnectToSem(&_semClient, semClientName);
    if (_semClient == SEM_FAILED) {
        sem_close(_semHost);
        throw std::runtime_error("sem_client wasn't opened");
    }

    signal(SIGUSR1, OnSignalRecieve);
    signal(SIGTERM, OnSignalRecieve);
    signal(SIGINT, OnSignalRecieve);

    syslog(LOG_INFO, "Goat was born");
    std::cout << "Goat was born" << std::endl;
}

Goat::~Goat() {
    if (_semHost != SEM_FAILED) {
        sem_close(_semHost);
    }

    if (_semClient != SEM_FAILED) {
        sem_close(_semClient);
    }

    syslog(LOG_INFO, "Goat was dead");
    std::cout << "Goat was dead" << std::endl;
    closelog();
}

Goat& Goat::GetInstance(int hostPid) {
    static Goat goat(hostPid);
    return goat;
}

void Goat::PrepareGame() noexcept{
    kill(_hostPid, SIGUSR1);
}

void Goat::StartGame() noexcept{
    std::cout << "Goat's started hiding..." << std::endl;

    std::cout << "___________GAME_STEP___________" << std::endl;
    if (!GenAndWriteValue()) {
        return;
    }

    if (!SemSignal(_semHost)) {
        return;
    }

    while (true) {
        std::cout << "___________GAME_STEP___________" << std::endl;

        if (!SemWait(_semClient)) {
            return;
        }

        Conn::Msg msg;
        memset(&msg, 0, sizeof(msg));

        if (!_conn.Read(&msg, sizeof(msg))) {
            return;
        }

        _goatState = static_cast<GOAT_STATE>(msg.data);

        if (!GenAndWriteValue()) {
            return;
        }

        if (!SemSignal(_semHost)) {
            return;
        }
    }
}

int Goat::GenerateValue() noexcept{
    std::random_device rd;
    std::mt19937 gen(rd());
    if (IsAlive()) {
        std::uniform_int_distribution<int> dis(GAME_CONSTANTS::MIN_GOAT_VAL_ALIVE, GAME_CONSTANTS::MAX_GOAT_VAL_ALIVE);
        return dis(gen);
    } else {
        std::uniform_int_distribution<int> dis(GAME_CONSTANTS::MIN_GOAT_VAL_DEAD, GAME_CONSTANTS::MAX_GOAT_VAL_DEAD);
        return dis(gen);
    }
}

bool Goat::GenAndWriteValue() noexcept{
    Conn::Msg msg;
    int cur_val = GenerateValue();
    std::cout << "Goat: My number is " << cur_val << std::endl;

    msg.type = Conn::MSG_TYPE::TO_WOLF;
    msg.data = cur_val;

    if (!_conn.Write(&msg, sizeof(msg))) {
        return false;
    }
    return true;
}

bool Goat::SemWait(sem_t* sem) noexcept{
    if (sem_wait(sem) == -1) {
        perror("sem_timewait() ");
        return false;
    }
    return true;
}

bool Goat::SemSignal(sem_t* sem) noexcept{
    if (sem_post(sem) == -1) {
        perror("sem_post() ");
        return false;
    }
    return true;
}

void Goat::ConnectToSem(sem_t** sem, std::string sem_name) noexcept{
    for (int i = 0; i < GAME_CONSTANTS::SEM_RECONNECT_TIMEOUT; i++) {
        *sem = sem_open(sem_name.c_str(), 0);
        if (*sem != SEM_FAILED) {
            break;
        }
        sleep(1);
    }
}

void Goat::OnSignalRecieve(int sig) noexcept{

    Goat &goat = GetInstance();

    switch (sig) {
    case SIGTERM:
    case SIGINT:
        syslog(LOG_INFO, "Terminate client (signal)");
        std::cout << "Terminate client (signal)" << std::endl;
        exit(EXIT_SUCCESS);
        break;
    case SIGUSR1:
        kill(goat._hostPid, SIGTERM);
        exit(EXIT_SUCCESS);
    default:
        syslog(LOG_INFO, "Unknown signal recieve: continue work");
        std::cout << "Unknown signal recieve: continue work" << std::endl;
        break;
    }
}
