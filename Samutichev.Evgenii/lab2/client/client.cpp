#include "client.h"
#include "../core/sys_exception.h"
#include "../core/game_constants.h"
#include <syslog.h>
#include <errno.h>
#include <fcntl.h>

const mode_t permissions = 0666;

Client::Client(size_t connectionID, Semaphore hostSem, Semaphore clientSem)
    : _hostSem(hostSem), _clientSem(clientSem) {
    _conn = Connection::create(connectionID, false);
}

void Client::work() {
    while(true) {
        _clientSem.wait();

        int temp = _goatling.responseToWolf();
        syslog(LOG_NOTICE, "  G %d", temp);
        _conn->write(temp);
        
        _hostSem.post();
        _clientSem.wait();
        
        temp = _conn->read();
        if (temp == Game::aliveCode)
            syslog(LOG_NOTICE, "  Alive");
        else
            syslog(LOG_NOTICE, "  Dead");
        _goatling.setStatus(temp);
    }
    _hostSem.post();
}

Client::~Client() {
    delete _conn;
}
