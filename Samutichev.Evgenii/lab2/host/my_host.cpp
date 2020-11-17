#include "my_host.h"
#include "../core/sys_exception.h"
#include "../core/game_constants.h"
#include "../client/client.h"
#include <unistd.h>
#include <iostream>
#include <syslog.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <signal.h>

Host* Host::_instance = nullptr;
const size_t connID = 777;
const char* hostSemName = "Wolfer";
const char* clientSemName = "Goatling";
const mode_t permissions = 0666;
const time_t timeout = 5;

Host* Host::get() {
    if (_instance == nullptr)
        _instance = new Host();
    return _instance;
}

Host::~Host() {
    delete _conn;
}

Host::Host()
    : _hostSem(hostSemName), _clientSem(clientSemName) {
    _currentTurn = 1;
    _conn = nullptr;
}

void Host::run() {
    pid_t pid = fork();

    if (pid < 0)
        throw SysException("Failed to fork", errno);

    if (pid == 0) {
        sleep(1);
        syslog(LOG_NOTICE, "Client process initialized");
        Client(connID, _hostSem, _clientSem).work();
    } else {
        _conn = Connection::create(connID, true);
        work();
        kill(pid, SIGTERM);
        syslog(LOG_NOTICE, "Child process terminated");
    }
}

void Host::work() {
    while(true) {
        printf("Turn %lu\n", _currentTurn);
        printf("Enter wolf number: ");
        int number;
        int status;
        std::cin >> number;
        if (number < 0) {
            printf("The game was closed\n");
            break;
        }
        syslog(LOG_NOTICE, "  W %d", number);

        if (!_wolfer.setWolferNumber(number)) {
            printf("Wolf number should be from 1 to %d\n\n", Game::wolferNumMax);
            continue;
        }

        _clientSem.post();
        if(!_hostSem.timedWait(timeout)) {
            printf("%ld seconds passed, no response from goatling!!!\n", timeout);
            break;
        }

        status = _wolfer.processGoatling(_conn->read());
        if (status == Game::errorCode) {
            printf("Goatling was caught cheating\n");
            break;
        }

        if (_wolfer.gameOver()) {
            printf("Game over!\n");
            break;
        }
        _conn->write(status);

        _currentTurn++;
        _clientSem.post();
    }
}
