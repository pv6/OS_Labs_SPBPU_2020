#include <iostream>
#include <fstream>
#include <string>
#include "cfg_parser.h"
#include "my_daemon.h"
#include "my_handler.h"
#include <syslog.h>
#include "types.h"

using std::string, std::cout, std::endl;

int main(int argc, char const * const argv[]) {
    try {
        if (argc == 2) {
            string cfgName = argv[1];
            DaemonBasePtr daemon;
            if (DaemonBase::CreateDaemon<Daemon>(cfgName, daemon) == PROCCESS_TYPE::PARENT)
                return 0;

            Handler::_pDaemon = daemon.get();
            daemon->SetSignalHandler(Handler::SignalHandler);
            daemon->process();
        }
        else {
            throw std::runtime_error("Must be only cfg name as run program param");
        }
    }
    catch (const std::out_of_range& ex) {
        openlog("MyDaemon", LOG_PID, LOG_DAEMON);
        syslog(LOG_ERR, "Can't find param in cfg file");
        closelog();
        return -1;
    }
    catch (const std::ifstream::failure& ex) {
        openlog("MyDaemon", LOG_PID, LOG_DAEMON);
        syslog(LOG_ERR, "%s", ex.what());
        closelog();
        return -1;
    }
    catch(const std::bad_any_cast& ex)
    {
        openlog("MyDaemon", LOG_PID, LOG_DAEMON);
        syslog(LOG_ERR, "%s", ex.what());
        closelog();
        return -1;
    }
    catch (const std::runtime_error& ex) {
        openlog("MyDaemon", LOG_PID, LOG_DAEMON);
        syslog(LOG_ERR, "%s", ex.what());
        closelog();
        return -1;
    }

    return 0;
}
