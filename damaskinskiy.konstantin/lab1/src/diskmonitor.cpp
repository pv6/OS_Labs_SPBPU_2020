#include <signal.h>
#include <sys/syslog.h>
#include <unistd.h>
#include <stdexcept>
#include "config.h"
#include "diskmonitor.h"

DiskMonitor::DiskMonitor( std::string const &configName ) : config(configName)
{
}

void DiskMonitor::start()
{
    const std::string tag = "DM_KD";

    if (fork() == -1)
        throw std::runtime_error("Start fork failed");

    // open system log
    openlog(tag.c_str(), LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_LOCAL0);

    // set up signal handling
    signal(SIGTERM, signalHandle);
    signal(SIGHUP, signalHandle);

    try
    {
        config.load();

        if (setsid() == -1)
            throw std::runtime_error("setsid fail. errno: " + std::to_string(errno));

        // create child process for background work handling
        switch (fork())
        {
        case -1:
            syslog(LOG_ERR, "Work process fork failed");
            terminate();
        case 0:
            syslog(LOG_INFO, "Created child work process");

            // start work handle loop
            startWorkLoop();
        }
    }
    catch (std::exception &exception)
    {
        syslog(LOG_ERR, "%s", exception.what());
        terminate();
        clear();
    }

    syslog(LOG_INFO, "Exit parent process");
    clear();
    return true;
}
