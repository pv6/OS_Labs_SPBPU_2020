//
// Created by aleksandr on 26.11.2020.
//



#include <cstring>
#include "wolf.h"
#include "../support/gameconst.h"
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <ctime>
#include <random>


Wolf &Wolf::getWolf() {
    static Wolf wolf;
    return wolf;
}

Wolf::Wolf(): client_info(0) {
    struct sigaction sig;
    memset(&sig, 0, sizeof(sig));
    sig.sa_sigaction = signalHandler;
    sig.sa_flags = SA_SIGINFO;
    sigaction(SIGTERM, &sig, nullptr);
    sigaction(SIGUSR1, &sig, nullptr);
    sigaction(SIGUSR2, &sig, nullptr);
    sigaction(SIGINT, &sig, nullptr);
}

bool Wolf::openConnection() {
    syslog(LOG_INFO, "Start connect...");

    if (!connection.openConnection(getpid(), true)){
        return false;
    }

    semaphore_host = sem_open(GameConst::SEM_HOST_NAME.c_str(),  O_CREAT, 0666, 0);
    semaphore_client = sem_open(GameConst::SEM_CLIENT_NAME.c_str(),  O_CREAT, 0666, 0);

    if (semaphore_host == SEM_FAILED || semaphore_client == SEM_FAILED){
        syslog(LOG_ERR, "%s", strerror(errno));
        return false;
    }

    syslog(LOG_INFO, "Connection is set");

    return true;
}

void Wolf::run() {
    struct timespec ts;
    Message msg;
    syslog(LOG_INFO, "Waiting for client to connect...");
    pause();
    syslog(LOG_INFO, "Client connected");

    sem_post(semaphore_client);

    while (status){
        if (!client_info.is_ready){
            syslog(LOG_INFO, "Waiting for client to connect...");
            sem_wait(semaphore_client);
            while (status && !client_info.is_ready) {
                pause();
            }
            if (!status) {
                return;
            }
            syslog(LOG_INFO, "Client connected");
            sem_post(semaphore_client);
        }
        else{
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += GameConst::TIMEOUT;
            if (sem_timedwait(semaphore_host, &ts) == -1){
                if (errno == EINTR){
                    status = false;
                    continue;
                }
                kill(client_info.pid, SIGTERM);
                client_info = ClientInfo(0);
                status = false;
                continue;
            }

            if (connection.readConnection(&msg)) {
                syslog(LOG_INFO, "The beginning of the round");
                syslog(LOG_INFO, "Goat number: %i", msg.number);
                syslog(LOG_INFO, "Goat status: %s", (msg.status == GOAT_STATUS::ALIVE) ? "Alive" : "Dead");

                getWolfNum();

                syslog(LOG_INFO, "Wolf number: %i", wolf_num);

                updateStatus(msg);

                syslog(LOG_INFO, "New goat status: %s", (msg.status == GOAT_STATUS::ALIVE) ? "Alive" : "Dead");

                if(client_info.deaths >= GameConst::MAX_DEATHS){
                    syslog(LOG_INFO, "Wolf win");

                    kill(client_info.pid, SIGTERM);
                    client_info = ClientInfo(0);
                    status = false;
                }
                else{
                    connection.writeConnection(&msg, sizeof(msg));
                }
            }
            sem_post(semaphore_client);
        }
    }
    terminate();
}

void Wolf::getWolfNum() {
    std::random_device rd;
    std::minstd_rand mt(rd());
    std::uniform_int_distribution<int> dist(1, GameConst::MAX_WOLF_NUM);
    wolf_num = dist(mt);
}

void Wolf::updateStatus(Message &answer) {
    GOAT_STATUS new_goat_status = GOAT_STATUS::ALIVE;

    if(answer.status == GOAT_STATUS::ALIVE){
        if(abs(wolf_num - answer.number) > 70){
            client_info.deaths += 1;
            new_goat_status = GOAT_STATUS::DEAD;
        }
    }
    else{
        if(abs(wolf_num - answer.number) > 20){
            client_info.deaths += 1;
            new_goat_status = GOAT_STATUS::DEAD;
        }
        else{
            client_info.deaths = 0;
        }
    }

    answer.status = new_goat_status;
}

void Wolf::terminate() {

    status = false;

    if (client_info.is_ready) {
        kill(client_info.pid, SIGTERM);
        client_info = ClientInfo(0);
    }

    if (semaphore_host != SEM_FAILED) {
        sem_unlink(GameConst::SEM_HOST_NAME.c_str());
    }

    if (semaphore_client != SEM_FAILED) {
        sem_unlink(GameConst::SEM_CLIENT_NAME.c_str());
    }


    if (!connection.closeConnection())
    {
        exit(errno);
    }

    syslog(LOG_INFO, "Wolf terminated");
}

void Wolf::signalHandler(int signum, siginfo_t *info, void *ptr) {
    static Wolf& wolf = getWolf();
    switch (signum) {
        case SIGUSR1:
            if (wolf.client_info.is_ready) {
                syslog(LOG_INFO, "Only one client is possible");
            } else {
                syslog(LOG_INFO, "Client with pid = %d connected", info->si_pid);
                wolf.client_info = ClientInfo(info->si_pid);
            }
            break;
        case SIGUSR2:
            syslog(LOG_INFO, "Client error");
            wolf.client_info = ClientInfo(0);
            break;
        case SIGTERM:
        case SIGINT:
            if (wolf.client_info.is_ready) {
                kill(wolf.client_info.pid, signum);
                wolf.client_info = ClientInfo(0);
            }
            wolf.terminate();
            break;
        default:
            syslog(LOG_INFO, "Unknown signal");
            break;
    }
}
