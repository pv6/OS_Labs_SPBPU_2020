#include "ConnectionInfo.h"
#include "ConnectionConst.h"

const std::string ConnectionConst::SEM_HOST_NAME = "LAB2_HOST";

const std::string ConnectionConst::SEM_CLIENT_NAME = "LAB2_CLIENT";

int ConnectionInfo::getPid() const {
    return pid;
}

bool ConnectionInfo::isAttached() const {
    return attached;
}

void ConnectionInfo::setAttached(bool isAttached) {
    this->attached = isAttached;
}
