#include <iostream>
#include <sys/msg.h>
#include <syslog.h>
#include <signal.h>

#include "conn.h"

Conn::Conn(int msgkey, bool create) {
    openlog("GameConnection", LOG_PID, LOG_DAEMON);
    _owner = create;
    _desc = msgget(msgkey, IPC_CREAT | 0666);
    if (_desc == -1) {
        throw std::runtime_error("msgget error");
    }

    if (_owner) {
        syslog(LOG_INFO, "msg queue created with qid %d", _desc);
        std::cout << "msg queue created with qid " << _desc << std::endl;
    } else {
        syslog(LOG_INFO, "msg queue created with qid %d", _desc);
        std::cout << "msg queue opened with qid " << _desc << std::endl;
    }
}

Conn::~Conn() {
    if (!_owner) {
        return;
    }

    if (msgctl(_desc, IPC_RMID, 0) == -1) {
        perror("msqctl() ");
    }

    syslog(LOG_INFO, "msgqueue closed qid: %d", _desc);
    std::cout << "msgqueue closed qid: " << _desc << std::endl;
    closelog();
}

bool Conn::Read(void* buf, size_t count) noexcept {
    if (msgrcv(_desc, buf, count - sizeof(long), 0, SA_RESTART) == -1) {
        perror("msgrcv() ");
        return false;
    }
    return true;
}

bool Conn::Write(void* buf, size_t count) noexcept {
    if (msgsnd(_desc, buf, count - sizeof(long), 0) == -1) {
        perror("msgsnd() ");
        return false;
    }

    return true;
}
