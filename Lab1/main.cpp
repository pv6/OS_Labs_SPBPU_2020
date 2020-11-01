#include <syslog.h>
#include <csignal>

#include "daemon.h"

int main(int argc,char **argv)
{
    Daemon* daemon = nullptr;
    try {
        daemon = Daemon::init(argc, argv);
        daemon->execute();
    }
    catch (const std::exception& err)
    {
        syslog(LOG_ERR, "%s. Work is stopping. ", err.what());
        return 1;
    }

    delete daemon;

    syslog(LOG_NOTICE, "Daemon was closed");
    return 0;
}