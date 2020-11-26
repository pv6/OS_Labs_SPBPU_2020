//
// Created by aleksandr on 26.11.2020.
//

#include <csignal>
#include <random>
#include "goat.h"
#include "../support/gameconst.h"
#include <cstring>
#include <iostream>

Goat::Goat(int host_pid) {
    this->host_pid = host_pid;
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);
}

Goat &Goat::getGoat(const int host_pid) {
    static Goat goat(host_pid);
    return goat;
}

void Goat::run() {

    struct timespec ts{};
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += GameConst::TIMEOUT;
    syslog(LOG_INFO, "Waiting for host to connect...");
    if (sem_timedwait(semaphore_client, &ts) == -1) {
        syslog(LOG_ERR, "Timeout");
        return ;
    }

    syslog(LOG_INFO, "Host connected");

    Message msg;

    GOAT_STATUS goat_status = GOAT_STATUS::ALIVE;

    msg.number = getNum(GameConst::MAX_ALIVE_NUM);
    msg.status = goat_status;

    //syslog(LOG_INFO, "Goat number: %i", msg.number);
    connection.writeConnection(&msg, sizeof(Message));

    sem_post(semaphore_host);



    while(status){

        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += GameConst::TIMEOUT;
        syslog(LOG_INFO, "Waiting status...");
        if (sem_timedwait(semaphore_client, &ts) == -1) {
            syslog(LOG_ERR, "Timeout");
            return ;
        }

        if(!status){
            syslog(LOG_INFO, "The host has finished working");
            continue;
        }


        if(!connection.readConnection(&msg, sizeof(Message))) {
            status = false;
            sem_post(semaphore_host);
            syslog(LOG_ERR, "Couldn't read the goat status");
            continue;
        }

        if(msg.status == GOAT_STATUS::ALIVE) {
            //syslog(LOG_INFO, "Goat is alive");
            msg.number = getNum(GameConst::MAX_ALIVE_NUM);
        }
        else {
            //syslog(LOG_INFO, "Goat is dead");
            msg.number = getNum(GameConst::MAX_DEAD_NUM);
        }

        //syslog(LOG_INFO, "Goat number: %i", msg.number);

        connection.writeConnection(&msg, sizeof(Message));

        sem_post(semaphore_host);

    }
}

bool Goat::openConnection() {
    syslog(LOG_INFO, "Start connect...");

    if (!connection.openConnection(host_pid, false)){
        return false;
    }

    semaphore_host = sem_open(GameConst::SEM_HOST_NAME.c_str(), 0);
    semaphore_client = sem_open(GameConst::SEM_CLIENT_NAME.c_str(), 0);

    if (semaphore_host == SEM_FAILED || semaphore_client == SEM_FAILED){
        syslog(LOG_ERR, "%s", strerror(errno));
        return false;
    }

    syslog(LOG_INFO, "Connection is set");
    kill(host_pid, SIGUSR1);

    return true;
}

int Goat::getNum(int max) {
    std::random_device rd;
    std::minstd_rand mt(rd());
    std::uniform_int_distribution<int> dist(1, max);
    return dist(mt);
}

void Goat::terminate() {
    kill(host_pid, SIGUSR2);

    if (errno != 0) {
        syslog(LOG_ERR, "Failing with error : %s", strerror(errno));
    }

    semaphore_host = SEM_FAILED;
    semaphore_client = SEM_FAILED;

    if (sem_close(semaphore_client) == -1 || sem_close(semaphore_host) == -1) {
        syslog(LOG_ERR, "Error in sem_close: %s", strerror(errno));
    }

    connection.closeConnection();
    status = false;
}

void Goat::signalHandler(int signum) {
    Goat& goat = Goat::getGoat(0);
    switch (signum) {
        case SIGTERM:
        case SIGINT:
            syslog(LOG_INFO, "Goat terminate");
            goat.status = false;
            break;
        default:
            break;

    }
}
