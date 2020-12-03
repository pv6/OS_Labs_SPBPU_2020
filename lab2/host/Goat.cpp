//
// Created by yudzhinnsk on 11/25/2020.
//

#include <syslog.h>
#include <csignal>
#include <cstring>
#include <unistd.h>
#include "Goat.h"

Goat* Goat::GetInstance(int host_pid) {
    static Goat self(host_pid);
    return &self;
}

Goat::Goat(int host_pid_) {
    syslog(LOG_NOTICE, "Host handler pid is: %d", host_pid);
    host_pid = host_pid_;
    srand(time(NULL) + host_pid_);
    signal(SIGTERM, SignalHandler);
}
Goat::Goat(Goat& other){}

Goat& Goat::operator=(Goat& other) {
    return other;
}

bool Goat::CreateConnection() {
    if (_connection.CreateConn(host_pid, false)) {
        sem_client_name = "client_" + std::to_string(host_pid);
        sem_host_name = "host_" + std::to_string(host_pid);

        semaphore_host = sem_open(sem_host_name.c_str(), 0);
        if (semaphore_host == SEM_FAILED) {
            syslog(LOG_ERR, "ERROR: client: can`t CreateConnect host semaphore (%s) with error %s", sem_host_name.c_str(), strerror(errno));
            return false;
        }
        syslog(LOG_NOTICE, "client: host semaphore opened (%s)", sem_host_name.c_str());

        semaphore_client = sem_open(sem_client_name.c_str(), 0);
        if (semaphore_client == SEM_FAILED) {
            syslog(LOG_ERR, "ERROR: client: can`t CreateConnect client semaphore (%s) with error %s", sem_client_name.c_str(), strerror(errno));
            return false;
        }
        syslog(LOG_NOTICE, "Host semaphore opened from client (%s)", sem_client_name.c_str());

        return true;
    }
    return false;
}

void Goat::StartRand() {
    syslog(LOG_INFO, "Goat rand() started");
    int goatStat;
    int res;
    while(true){
        sem_wait(semaphore_client);
        if (_connection.Read(&goatStat, sizeof(int))) {
            if(goatStat){
                res = 1 + rand() % 50;
            }else{
                res = 1 + rand() % 20;
            }
            _connection.Write(&res, sizeof(int));
            sem_post(semaphore_host);
        }
    }
}


void Goat::Terminate(int signum) {
    syslog(LOG_NOTICE, "Termination of client(%d) started...", getpid());
    if (_connection.CloseConn()) {
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





void Goat::SignalHandler(int signum) {
    Goat *instance = Goat::GetInstance(getpid());
    switch (signum) {
        case SIGTERM: {
            instance->Terminate(signum);
        }
    }
}

void Goat::set_conn(Conn connection_) {
    _connection = connection_;
}