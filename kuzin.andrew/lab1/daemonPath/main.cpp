#include "daemon.h"

int main(int argc, char *argv[])
{
    Daemon& daemon = Daemon::getInstance();

    try
    {
        daemon.run(argc, argv);
    } catch (const DaemonException &e)
    {
        syslog(LOG_ERR, "%s. Stopping reminder.", e.getMsg().c_str());
        closelog();
        return e.isFatalError();
    }
    return 0;
}
