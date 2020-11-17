#include "my_host.h"
#include "../core/sys_exception.h"
#include "../core/game_constants.h"
#include <unistd.h>
#include <iostream>
#include <syslog.h>
#include <errno.h>
#include <time.h>
#include <signal.h>

const mode_t permissions = 0666;
const time_t timeout = 5;

Host::Host(size_t connectionID) {
    _currentTurn = 1;
    _conn = Connection::create(connectionID, true);
}

Host::~Host() {
    delete _conn;
}

void Host::run(Semaphore& hostSem, Semaphore& clientSem) {
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

        clientSem.post();
        if(!hostSem.timedWait(timeout)) {
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
        clientSem.post();
    }
}
