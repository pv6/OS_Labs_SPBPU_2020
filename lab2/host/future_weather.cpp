//
// Created by Daria on 11/25/2020.
//
#include "future_weather.h"
#include <syslog.h>
#include <fcntl.h>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include <iostream>

future_weather* future_weather::get_instance(int id) {
    static future_weather self(id);
    return &self;
}

future_weather::future_weather(int id) {
    syslog(LOG_NOTICE, "Client id is: %d", id);
    client_id = id;
    srand(time(NULL) + id);
    rand_offset = rand();
    signal(SIGTERM, signal_handler);
}
future_weather::future_weather(future_weather& other){}

future_weather& future_weather::operator=(future_weather& other) {
    return other;
}

bool future_weather::open_connection() {
    if (connection.open(client_id, false)) {
        sem_client_name = "client_" + std::to_string(client_id);
        sem_host_name = "host_" + std::to_string(client_id);

        semaphore_host = sem_open(sem_host_name.c_str(), 0);
        if (semaphore_host == SEM_FAILED) {
            syslog(LOG_ERR, "ERROR: client: can`t open host semaphore (%s) with error %s", sem_host_name.c_str(), strerror(errno));
            return false;
        }
        syslog(LOG_NOTICE, "client: host semaphore opened (%s)", sem_host_name.c_str());

        semaphore_client = sem_open(sem_client_name.c_str(), 0);
        if (semaphore_client == SEM_FAILED) {
            syslog(LOG_ERR, "ERROR: client: can`t open client semaphore (%s) with error %s", sem_client_name.c_str(), strerror(errno));
            return false;
        }
        syslog(LOG_NOTICE, "Host semaphore opened from client (%s)", sem_client_name.c_str());

        return true;
    }
    return false;
}

void future_weather::start() {
    syslog(LOG_INFO, "Weather calculating started");
    message msg;
    message buf;
    while (true) {
        sem_wait(semaphore_client);
        if (connection.read(&buf, sizeof(buf))) {
            msg.set_temp(get_weather(buf.get_day(), buf.get_month(), buf.get_year()));
            connection.write(&msg, sizeof(msg));
            sem_post(semaphore_host);
        }
    }
}


void future_weather::terminate(int signum) {
    syslog(LOG_NOTICE, "Termination of client(%d) started...", getpid());
    if (connection.close()) {
        if (semaphore_host != SEM_FAILED) {
            semaphore_host = SEM_FAILED;
            sem_close(semaphore_host);
        }
        if (semaphore_client != SEM_FAILED) {
            semaphore_client = SEM_FAILED;
            sem_close(semaphore_client);
        }
        exit(signum);
    }
    syslog(LOG_ERR, "Terminating error: %s", strerror(errno));
    exit(errno);
}


int future_weather::get_weather(int day, int month, int year) {
    srand((day * 31 + month * 12 + year * 366 + rand_offset) % INTMAX_MAX);
    int temp = rand() % 100;
    int sig = (rand() % 2 == 0) ? -1 : 1;
    return temp * sig;
}


void future_weather::signal_handler(int signum) {
    future_weather *instance = future_weather::get_instance(getpid());
    switch (signum) {
        case SIGTERM: {
            instance->terminate(signum);
        }
    }
}

void future_weather::set_conn(conn connection_) {
    connection = connection_;
}
