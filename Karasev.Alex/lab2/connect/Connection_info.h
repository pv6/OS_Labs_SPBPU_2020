#ifndef CONNECTIONINFO_H
#define CONNECTIONINFO_H

#include <string>

static const std::string sem_host_name = "LAB2_HOST";
static const std::string sem_client_name = "LAB2_CLIENT";
static const int timeout = 5;

class ConnectionInfo {
public:
    ConnectionInfo(int pid) : pid{pid}, attached{pid != 0} {}

    int getPid() const;

    bool isAttached() const;

    void setAttached(bool isAttached);

private:
    int pid;
    bool attached;
};


#endif //CONNECTIONINFO_H