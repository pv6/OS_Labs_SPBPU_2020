#include <cstring>
#include <cstdio>

#include <string>

#include <syslog.h>
#include <unistd.h>

#include "predictor.h"

int main( int argc, char *argv[] )
{
    // this is predictor (client) pid!
    char pidstr[10];
    sprintf(pidstr, "%i", getpid());
    openlog("DK_forecast_predictor",
            LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_USER);

    // and this is host pid!
    if (argc != 3)
    {
        syslog(LOG_INFO, "Usage: "
                         "--pid process id\n");
        return -1;
    }

    if (argc >= 2 && strcmp(argv[1], "--pid"))
    {
        syslog(LOG_INFO, "Usage: "
                         "--pid process id\n");
        return -1;
    }

    int pid;
    try
    {
        pid = std::stoi(argv[2]);
    }
    catch (std::exception &) {
        syslog(LOG_ERR, "Bad host pid passed!");
        closelog();
        return -1;
    }

    Predictor &pred = Predictor::get();
    pred.setHostPid(pid);

    if (pred.connectToHost())
    {
        syslog(LOG_INFO, "Successfuly connected predictor %i to host!", getpid());
        pred.predict();
        syslog(LOG_INFO, "Prediction from %i finished!", getpid());
        closelog();
    }
    else
    {
        syslog(LOG_ERR, "Failed to connect to host");
        pred.terminate();
    }

    return 0;
}
