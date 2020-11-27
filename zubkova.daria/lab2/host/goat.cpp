#include <unistd.h>
#include <csignal>
#include <iostream>
#include <semaphore.h>
#include "goat.h"
#include <random>

Goat* Goat::GetInstance(int id) {
    static Goat instance(id);
    return &instance;
}

Goat::Goat(int id) {
    myId = id;
    status = Status::ALIVE;
    signal(SIGTERM, SignalHandler);
}

void Goat::Set(conn* connection, sem_t* semHost, sem_t* semClient){
    this->connection = connection;
    this->semHost = semHost;
    this->semClient = semClient;
}

Goat::~Goat(){
    if (!connection->Close()) {
        syslog(LOG_ERR, "ERROR: %s", strerror(errno));
    }
    if (semClient != nullptr && semHost != nullptr) {
        std::string semName = "sem_client_" + std::to_string(myId);
        std::string semName2 = "sem_host_" + std::to_string(myId);
        if (sem_unlink(semName.c_str()) == -1 || sem_unlink(semName2.c_str()) == -1) {
            syslog(LOG_ERR, "ERROR: %s", strerror(errno));
        }
    }
}

Status Goat::GetStatus(){
    return status;
}

conn* Goat::GetConnection(){
    return connection;
}

sem_t* Goat::GetSemHost(){
    return semHost;
}

sem_t* Goat::GetSemClient(){
    return semClient;
}

pid_t Goat::GetPid(){
    return pid;
}

void Goat::SetPid(pid_t p) {
    this->pid = p;
}

void Goat::Start() {
    while (st) {
        sem_wait(semClient);
        Message* buf = new Message();
        if (!connection->Read(buf, sizeof(*buf))){
            //std::cout << "Can't read in goat" << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: Can't read in goat %s", strerror(errno));
            break;
        }
        if (buf->owner == Owner::WOLF) {
            status = buf->st;
            int num = GenerateRandomNumber(status == Status::ALIVE ? 100 : 50);
            buf = new Message(Owner::GOAT, status, num);
        }
        if (!connection->Write(buf, sizeof(*buf))){
            //std::cout << "Can't write in goat" << strerror(errno) << std::endl;
            syslog(LOG_ERR, "ERROR: Can't write in goat %s", strerror(errno));
            break;
        }
        sem_post(semHost);
    }
    Terminate();
}

void Goat::Terminate(){
    if (!connection->Close()) {
        syslog(LOG_ERR, "ERROR: %s", strerror(errno));
    }
    if (semClient != nullptr && semHost != nullptr) {
        std::string semName = "sem_client_" + std::to_string(myId);
        std::string semName2 = "sem_host_" + std::to_string(myId);
        if (sem_unlink(semName.c_str()) == -1 || sem_unlink(semName2.c_str()) == -1) {
            syslog(LOG_ERR, "ERROR: %s", strerror(errno));
        }
        semClient = nullptr;
        semHost = nullptr;
    }
}

void Goat::SignalHandler(int signum) {
    Goat *instance = Goat::GetInstance(getpid());
    switch (signum) {
        case SIGTERM: {
            instance->st = false;
            instance->Terminate();
        }
    }
}

int Goat::GenerateRandomNumber(int max) {
    std::random_device rd;
    std::minstd_rand mt(rd());
    std::uniform_int_distribution<int> dist(1, max);
    return dist(mt);
}