//
// Created by Evgenia on 08.11.2020.
//

#include "ClientHostConnection.h"

bool ClientHostConnection::HasSignalledBack() {
    return signal_back;
}

int ClientHostConnection::GetClientPid() {
    return client_pid;
}

sem_t &ClientHostConnection::GetSemaphore() {
    return *semaphore;
}

sem_t &ClientHostConnection::GetHostSemaphore() {
    return *semaphore_host;
}

bool ClientHostConnection::SetupConnection(int id) {
    client_id = id;
    std::string semName = 's' + std::to_string(client_id);
    std::string semName2 = 'h' + std::to_string(client_id);
    sem_unlink(semName.c_str());
    sem_unlink(semName2.c_str());
    semaphore = sem_open(semName.c_str(), O_CREAT, 0666, 0);
    semaphore_host = sem_open(semName2.c_str(), O_CREAT, 0666, 0);
    if (connection.Open(id, true)) {
        if (semaphore == SEM_FAILED || semaphore_host == SEM_FAILED) {
            syslog(LOG_ERR, "%s", strerror(errno));
            return false;
        } else {
            return true;
        }
    }
    return false;
}

void ClientHostConnection::OnSignalBack(int pid) {
    this->signal_back = true;
    this->client_pid = pid;
}



Conn &ClientHostConnection::GetConnection() {
    return connection;
}

void ClientHostConnection::Delete() {
    if (semaphore == nullptr)
        return;
    std::string semName = 's' + std::to_string(client_id);
    std::string semName2 = 'h' + std::to_string(client_id);
    if (sem_unlink(semName.c_str()) == -1 || sem_unlink(semName2.c_str()) == -1) {
        syslog(LOG_ERR, "%s", strerror(errno));
    }
    if (!connection.Close()) {
        syslog(LOG_ERR, "%s", strerror(errno));
    }
    semaphore = nullptr;
}

void ClientHostConnection::SetAlive() {
    days_dead = 0;
}

void ClientHostConnection::IncrDaysDead() {
    days_dead++;
}

int ClientHostConnection::GetDaysDead() {
    return days_dead;
}
