#include "client.h"
#include "../core/sys_exception.h"
#include "../core/game_constants.h"
#include <syslog.h>
#include <errno.h>
#include <fcntl.h>

const mode_t permissions = 0666;

Client::Client(size_t connectionID) {
    _conn = Connection::create(connectionID, false);
}

void Client::run(Semaphore& hostSem, Semaphore& clientSem) {
    while(true) {
        clientSem.wait();

        int temp = _goatling.responseToWolf();
        syslog(LOG_NOTICE, "  G %d", temp);
        _conn->write(temp);

        hostSem.post();
        clientSem.wait();

        temp = _conn->read();
        if (temp == Game::aliveCode)
            syslog(LOG_NOTICE, "  Alive");
        else
            syslog(LOG_NOTICE, "  Dead");
        _goatling.setStatus(temp);
    }
}

Client::~Client() {
    delete _conn;
}
