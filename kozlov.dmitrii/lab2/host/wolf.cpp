#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <syslog.h>
#include <signal.h>
#include <unistd.h>

#include "wolf.h"

Wolf::Wolf(int hostPid)
    : _conn(hostPid, true) {

    openlog("GameHost", LOG_PID, LOG_DAEMON);

    _semHostName = std::string("host_" + std::to_string(_hostPid));
    _semClientName = std::string("client_" + std::to_string(_hostPid));

    _isClientConnected = false;
    _clientPid = 0;
    _hostPid = hostPid;

    _pSemHost = sem_open(_semHostName.c_str(), O_CREAT, 0666, 0);
    if (_pSemHost == SEM_FAILED) {
        throw std::runtime_error("sem_host wasn't opened");
    }

    _pSemClient = sem_open(_semClientName.c_str(), O_CREAT, 0666, 0);
    if (_pSemClient == SEM_FAILED) {
        sem_unlink(std::string("host_" + std::to_string(_hostPid)).c_str());
        throw std::runtime_error("sem_client wasn't opened");
    }

    syslog(LOG_INFO, "Wolf was born");
    std::cout << "Wolf was born" << std::endl;
}


Wolf::~Wolf() {
    if (_isClientConnected) {
        kill(_clientPid, SIGTERM);
    }

    if (_pSemHost != SEM_FAILED) {
        sem_unlink(_semHostName.c_str());
    }

    if (_pSemClient != SEM_FAILED) {
        sem_unlink(_semClientName.c_str());
    }

    syslog(LOG_INFO, "Wolf has won the game");
    std::cout << "Wolf has won the game" << std::endl;
    closelog();
}


Wolf& Wolf::GetInstance(int host_pid) {
    static Wolf wolf(host_pid);
    return wolf;
}


void Wolf::PrepareGame() noexcept{
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = HandleSignal;
    sigaction(SIGTERM, &sa, nullptr);
    sigaction(SIGUSR1, &sa, nullptr);
    sigaction(SIGUSR2, &sa, nullptr);
    sigaction(SIGINT, &sa, nullptr);
    pause();
}


void Wolf::StartGame() noexcept{
    syslog(LOG_INFO, "Wolf's started hunting...");
    std::cout << "Wolf's started hunting..." << std::endl;

    while (_goatState != GOAT_STATE::DEAD) {
        Conn::Msg msg;

        if (!SemWait(_pSemHost)) {
            return;
        }

        std::cout << "___________GAME_STEP___________" << std::endl;

        if (!_conn.Read(&msg, sizeof(msg))) {
            return;
        }

        int goatVal = msg.data;
        std::cout << "Goat: My num is: " << goatVal << std::endl;

        int curVal = GetValue();
        std::cout << "Wolf: My num is " << curVal << std::endl;

        if (_goatState == GOAT_STATE::ALIVE && abs(goatVal - curVal) > GAME_CONSTANTS::ALIVE_DEVIATION) {
            _goatState = GOAT_STATE::HALF_DEAD;
        }
        else if (_goatState == GOAT_STATE::HALF_DEAD) {
            if (abs(goatVal - curVal) > GAME_CONSTANTS::DEAD_DEVIATION)
                _goatState = GOAT_STATE::DEAD;
            else
                _goatState = GOAT_STATE::ALIVE;
        }

        switch (_goatState) {
        case GOAT_STATE::ALIVE:
            std::cout << "Goat: I am still alive, haha" << std::endl;
            break;
        case GOAT_STATE::HALF_DEAD:
            std::cout << "Goat: Oh, Wolf damaged me, but I have a chance to be revived" << std::endl;
            break;
        case GOAT_STATE::DEAD:
            std::cout << "Goat: Ops, I'm compleatly dead" << std::endl;
            break;
        }

        msg.type = Conn::MSG_TYPE::TO_GOAT;
        msg.data = static_cast<long>(_goatState);
        if (!_conn.Write(&msg, sizeof(msg))) {
            return;
        }

        if (!SemSignal(_pSemClient)) {
            return;
        }
    }
}


int Wolf::GetValue() noexcept{
    int res;
    std::cout << "Enter the number in range from " << GAME_CONSTANTS::MIN_WOLF_VAL << " to " << GAME_CONSTANTS::MAX_WOLF_VAL << std::endl;

    do {
        std::cin >> res;
        if (std::cin.fail()) {
            std::cin.clear();
        }
    } while (res < GAME_CONSTANTS::MIN_WOLF_VAL || res > GAME_CONSTANTS::MAX_WOLF_VAL);

    return res;
}


bool Wolf::SemWait(sem_t* sem) noexcept{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += GAME_CONSTANTS::TIMEOUT;
    if (sem_timedwait(sem, &ts) == -1) {
        perror("sem_timewait()");
        return false;
    }

    return true;
}


bool Wolf::SemSignal(sem_t* sem) noexcept{
    if (sem_post(sem) == -1) {
        perror("sem_post()");
        return false;
    }
    return true;
}

void Wolf::HandleSignal(int sig, siginfo_t* info, void* /*ptr*/) noexcept{
    static Wolf& wolf = GetInstance(0);
    switch (sig) {
    case SIGUSR1:
        if (wolf._isClientConnected) {
            syslog(LOG_INFO, "Ignore handshake, client has already connected");
            std::cout << "Ignore handshake, client has already connected" << std::endl;
            kill(info->si_pid, SIGTERM);
        } else {
            syslog(LOG_INFO, "Client connected: pid is %d", info->si_pid);
            std::cout << "Client connected: pid is " << info->si_pid << std::endl;
            wolf._isClientConnected = true;
            wolf._clientPid = info->si_pid;
        }
        break;
    case SIGUSR2:
        syslog(LOG_INFO, "The error has occured on client side");
        std::cout << "The error has occured on client side" << std::endl;
        wolf._isClientConnected = false;
        wolf._clientPid = 0;
        break;
    case SIGTERM:
    case SIGINT:
        syslog(LOG_INFO, "Host is terminated");
        std::cout << "Host is terminated" << std::endl;
        exit(EXIT_SUCCESS);
        break;
    default:
        syslog(LOG_INFO, "Unknown signal: continue work");
        std::cout << "Unknown signal: continue work" << std::endl;
        break;
    }
}
