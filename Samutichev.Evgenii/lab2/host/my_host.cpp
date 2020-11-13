#include "my_host.h"
#include "../core/sys_exception.h"
#include <unistd.h>
#include <iostream>
#include <syslog.h>
#include <semaphore.h>
#include <fcntl.h>

Host* Host::_instance = nullptr;
const size_t connID = 777;
const char* hostSemName = "Wolfer";
const char* clientSemName = "Goatling";
const mode_t permissions = 0666;

Host* Host::get() {
    if (_instance == nullptr)
        _instance = new Host();
    return _instance;
}

Host::~Host() {
    sem_close(_clientSem);
    sem_close(_hostSem);
    delete _conn;
}

Host::Host() {
    _currentTurn = 1;
    _conn = Connection::create(connID, true);

    syslog(LOG_NOTICE, "Creating host semaphore");
    _hostSem = sem_open(hostSemName, O_CREAT, permissions, 0);
    if (_hostSem == SEM_FAILED)
        throw SysException("Failed to create host semaphore", errno);

    syslog(LOG_NOTICE, "Creating client semaphore");
    _clientSem = sem_open(clientSemName, O_CREAT, permissions, 0);
    if (_clientSem == SEM_FAILED)
        throw SysException("Failed to create client semaphore", errno);
}

void Host::run() {
    pid_t pid = fork();

    if (pid < 0)
        throw SysException("Failed to fork", errno);

    if (pid == 0) {
        syslog(LOG_NOTICE, "Client process initialized");

    } else {
        work();
    }
}

void Host::work() {
    while(true) {
        std::cout << "Turn " << _currentTurn << "\n";
        _currentTurn++;
    }
}
