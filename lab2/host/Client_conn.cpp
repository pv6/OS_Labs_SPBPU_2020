//
// Created by yudzhinnsk on 11/25/2020.
//

#include "Client_conn.h"
#include "../conn/Conn.h"
#include <fcntl.h>
#include <cstring>
#include <unistd.h>
#include <csignal>
#include <semaphore.h>
#include <syslog.h>
#include <iostream>

Client_conn::Client_conn(int id) {
    _clientId = id;
}

Client_conn& Client_conn::operator=(Client_conn& c) {
    return c;
}

void Client_conn::SetClient(int pid) {
    _clientPid = pid;
    syslog(LOG_INFO, "Setting client w/ pid: %d", pid);
    _isConnected = (_clientPid != 0);
}

Conn Client_conn::GetConn() {
    return _conn;
}

void Client_conn::SetupServerConnection(Host_conn* connection) {
    _hostConn = connection;
    _hostConn->CreateConnect(_clientId);
}

bool Client_conn::OpenConnection() {
    if (_conn.CreateConn(_clientId, true)) {
        sem_client_name = "client_" + std::to_string(_clientId);
        sem_host_name = "host_" + std::to_string(_clientId);

        semaphore_host = sem_open(sem_host_name.c_str(), O_CREAT, 0666, 0);
        if (semaphore_host == SEM_FAILED) {
            syslog(LOG_ERR, "ERROR: handler: can`t CreateConnect host semaphore error = %s", strerror(errno));
            return false;
        }
        syslog(LOG_NOTICE, "handler: host semaphore created (%s)", sem_host_name.c_str());

        semaphore_client = sem_open(sem_client_name.c_str(), O_CREAT, 0666, 0);
        if (semaphore_client == SEM_FAILED) {
            syslog(LOG_ERR, "ERROR: handler: can`t CreateConnect client semaphore error = %s", strerror(errno));
            return false;
        }
        syslog(LOG_NOTICE, "handler: client semaphore created (%s)", sem_client_name.c_str());
        return true;
    }
    return false;
}

void Client_conn::SetTID(pthread_t tid) {
    _tid = tid;
}

pthread_t Client_conn::get_tid() {
    return _tid;
}

int Client_conn::GetID() {
    return _clientId;
}

void Client_conn::KillClient() {
    if (_isConnected) {
        syslog(LOG_NOTICE, "Kill client");
        kill(_clientPid, SIGTERM);
    }
}

void Client_conn::Terminate(int signum) {
    if (_isClosed)
        return;
    _isClosed = true;
    syslog(LOG_NOTICE, "Termination of handler w/ id %d", _clientId);
    KillClient();

    if (!_conn.CloseConn()) {
        syslog(LOG_ERR, "Terminating error _connection: %s", strerror(errno));
    }
    if (semaphore_host != SEM_FAILED) {
        semaphore_host = SEM_FAILED;
        if (sem_unlink(sem_host_name.c_str()) != 0) {
            syslog(LOG_ERR, "Terminating error sem_host_name: %s", strerror(errno));
        }
    }

    if (semaphore_client != SEM_FAILED) {
        semaphore_client = SEM_FAILED;
        if (sem_unlink(sem_client_name.c_str()) != 0) {
            syslog(LOG_ERR, "Terminating error sem_client_name: %s", strerror(errno));
        }
    }
    syslog(LOG_NOTICE, "Handler %d was killed", _clientId);
}

bool Client_conn::GetAliveStat(int &val) {
    int num;
    if (_hostConn->Read(&num, sizeof(num))) {
        val = num;
        return true;
    }
    return false;
}

void Client_conn::Start() {
    struct timespec ts;
    int res;
    int stat;
    int answ;
    while (true) {
        if (_hostConn->HasSign()) {
            _hostConn->SetReady(false);
            GetAliveStat(stat);
            if (_conn.Write(&stat, sizeof(int))) {
                sem_post(semaphore_client);
                clock_gettime(CLOCK_REALTIME, &ts);
                ts.tv_sec += timeout;
                res = sem_timedwait(semaphore_host, &ts);
                if (res == -1) {
                    KillClient();
                    Terminate(errno);
                    return;
                } else {
                    if (_conn.Read(&answ, sizeof(int))) {
                        _hostConn->SetReady(true);
                        _hostConn->Write(&answ, sizeof(int));
                    }
                }
            }
        }
    }
}

