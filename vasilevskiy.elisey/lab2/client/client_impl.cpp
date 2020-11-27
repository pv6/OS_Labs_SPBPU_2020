#include <semaphore.h>
#include <cstring>
#include "client.h"
#include <ConnectionConst.h>
#include <iostream>
#include <stdexcept>
#include <syslog.h>
#include <random>

Client &Client::getInstance(int host_pid) {
    static Client inst(host_pid);
    return inst;
}

Client::Client(int host_pid) {
    this->host_pid = host_pid;
    signal(SIGTERM, handleSignal);
    signal(SIGINT, handleSignal);
}

void Client::openConnection() {
    syslog(LOG_INFO, "try connect");
    connection.openConnection(host_pid, false);
    semaphore_host = sem_open(ConnectionConst::SEM_HOST_NAME.c_str(), 0);
    semaphore_client = sem_open(ConnectionConst::SEM_CLIENT_NAME.c_str(), 0);
    if (semaphore_host == SEM_FAILED || semaphore_client == SEM_FAILED) {
        throw std::runtime_error("sem_open failed with error " + std::string(strerror(errno)));
    } else {
        kill(host_pid, SIGUSR1);
    }

}

void Client::terminate() {
    kill(host_pid, SIGUSR2);
    if (errno != 0) {
        syslog(LOG_ERR, "disconnect error : %s",strerror(errno));
    }
    semaphore_host = SEM_FAILED;
    semaphore_client = SEM_FAILED;
    if (sem_close(semaphore_client) == -1 || sem_close(semaphore_host) == -1) {
        syslog(LOG_ERR, "sem_close error : %s", strerror(errno));
    }
    connection.closeConnection();
}

void Client::start() {
    timespec ts{};
    DTO dto;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += ConnectionConst::TIMEOUT;
    syslog(LOG_INFO, "wait host connect");
    if (sem_timedwait(semaphore_client, &ts) == -1) {
        throw std::runtime_error("timeout  " + std::string(strerror(errno)));
    }
    syslog(LOG_INFO, "host connected");
    work = true;
    while (work) {
        syslog(LOG_INFO, "wait host data");
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += ConnectionConst::TIMEOUT_CLIENT;
        if (sem_timedwait(semaphore_client, &ts) == -1){
            syslog(LOG_INFO, "host timeout");
            sem_post(semaphore_host);
            work = false;
            return;
        }
        syslog(LOG_INFO, "do work");
        connection.readConnection(&dto);
        int weather = getWeather(dto);
        dto.setTemp(weather);
        syslog(LOG_INFO, "weather: %i", weather);
        connection.writeConnection(&dto);
        syslog(LOG_INFO, "call host" );
        sem_post(semaphore_host);
    }
}

int Client::getWeather(const DTO &dto) {
    long date = dto.getYear() * 10000 + dto.getMonth() * 100 + dto.getDay();
    auto it = oldWeather.find(date);
    if (it != oldWeather.end()) {
        return it->second;
    }
    std::minstd_rand generator(date);
    std::uniform_int_distribution<int> distribution(ConnectionConst::MIN_TEMPERATURE, ConnectionConst::MAX_TEMPERATURE);
    int weather = distribution(generator);
    oldWeather.insert(std::pair<long, int>(date, weather));
    return weather;
}

void Client::handleSignal(int signum) {
    Client &client = getInstance(0);
    switch (signum)
    {
    case SIGTERM:
    case SIGINT:
        syslog(LOG_INFO, "stop work");
        client.work = false;
        break;
    default:
        break;
    }
}
