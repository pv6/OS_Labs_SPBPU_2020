#ifndef HOSTMANAGER_H_INCLUDED__
#define HOSTMANAGER_H_INCLUDED__

#include "../include/UserCommunion.h"
#include "../include/ConnectionManager.h"
#include <stddef.h>
#include <vector>

class HostManager {
public:
    HostManager();

    void run();


private:
    UserCommunion _uc;
    size_t _clients_num;
    int _date[3];
    std::vector<ConnectionManager *> _cms;

    static void *_run_connection_manager(void *cm);
};

#endif