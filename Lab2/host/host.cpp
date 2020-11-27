#include <syslog.h>
#include <signal.h>
#include <semaphore.h>

#include <cstdlib>
#include <string>

#include "../utils/SemaphoreWrapper.h"
#include "../utils/MyException.h"
#include "Wolf.h"
#include "Goat.h"

const size_t ID = 101;
const char* SEM_HOST_NAME = "ShrMemHost";
const char* SEM_SERVER_NAME = "ShrMemServer";

int main()
{
    openlog("game", LOG_CONS | LOG_PID, LOG_USER);
    syslog(LOG_INFO, "Host run");

    try {
        SemaphoreWrapper sem_host(SEM_HOST_NAME);
        SemaphoreWrapper sem_server(SEM_SERVER_NAME);

        Conn conn(ID, true);

        pid_t pid = fork();

        if (pid == -1)
            throw MyException("Fork failed");

        if (pid != 0) {
            syslog(LOG_INFO, "Host run client");

            Wolf& wolf = Wolf::getInstance(ID);
            wolf.run(sem_host, sem_server);

            kill(pid, SIGTERM);
            syslog(LOG_INFO, "Client completed.");
        } else {
            //sleep(1);

            Goat &goat = Goat::getInstance(ID);
            goat.run(sem_host, sem_server);
        }
    }
    catch(MyException& ex) {
        printf("Error occured: %s", ex.what());
        syslog(LOG_ERR, "Error occured: %s", ex.what());
        return 1;
    }

    return 0;
}
