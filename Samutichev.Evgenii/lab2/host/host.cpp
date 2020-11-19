#include "my_host.h"
#include "../core/sys_exception.h"
#include "../core/my_semaphore.h"
#include "../client/client.h"
#include <iostream>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

const size_t connID = 777;
const char* hostSemName = "Wolfer";
const char* clientSemName = "Goatling";

int main() {
    openlog("lab2_log", LOG_PID, LOG_DAEMON);

    Semaphore hostSem(hostSemName);
    Semaphore clientSem(clientSemName);

    try {
        pid_t pid = fork();
        if (pid < 0)
            throw SysException("Failed to fork", errno);

        if (pid == 0) {
            sleep(1);
            syslog(LOG_NOTICE, "Client process initialized");
            Client(connID).run(hostSem, clientSem);
        } else {
            Host(connID).run(hostSem, clientSem);
            kill(pid, SIGTERM);
            syslog(LOG_NOTICE, "Client process terminated");
        }

    } catch (const SysException& ex) {
        syslog(LOG_ERR, "%s", ex.what());
        std::cout << "An error occured, see syslog for details\n";
    }

    closelog();
}
