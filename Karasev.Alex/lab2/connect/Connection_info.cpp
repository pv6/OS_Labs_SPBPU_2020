#include "Connection_info.h"
#include <string>

int ConnectionInfo::getPid() const {
    return pid;
}

bool ConnectionInfo::isAttached() const {
    return attached;
}

void ConnectionInfo::setAttached(bool isAttached) {
    this->attached = isAttached;
}