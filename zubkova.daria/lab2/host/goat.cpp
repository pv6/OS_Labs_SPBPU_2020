#include <unistd.h>
#include <csignal>
#include <iostream>
#include <semaphore.h>
#include "goat.h"
#include <random>

Goat* Goat::GetInstance(int pid) {
    static Goat instance(pid);
    return &instance;
}

Goat::Goat(int pid) {
    myPid = pid;
    status = Status::ALIVE;
    signal(SIGTERM, SignalHandler);
}

Goat::Goat(Goat& other){}

Goat& Goat::operator=(Goat& other) {
    return other;
}

void Goat::Set(conn* connection, sem_t* semHost, sem_t* semClient, int id){
    this->connection = connection;
    this->semHost = semHost;
    this->semClient = semClient;
    this->myId = id;
}

Goat::~Goat(){}

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
    return myPid;
}

void Goat::Start() {
    while (st) {
        sem_wait(semClient);
        if (!st){
            break;
        }
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
}

void Goat::Terminate(){
    if (connection != nullptr) {
        if (!connection->Close()) {
            syslog(LOG_ERR, "ERROR: %s", strerror(errno));
        }
        connection = nullptr;
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
            break;
        }
    }
}

int Goat::GenerateRandomNumber(int max) {
    std::random_device rd;
    std::minstd_rand mt(rd());
    std::uniform_int_distribution<int> dist(1, max);
    return dist(mt);
}