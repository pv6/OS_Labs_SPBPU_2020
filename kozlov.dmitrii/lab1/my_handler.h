#pragma once
#include "my_daemon.h"

struct Handler {
    inline static DaemonBase * _pDaemon;
    static void SignalHandler(int sig)
    {
        if (!_pDaemon){
            syslog(LOG_INFO, "DaemonPtr is not valid");
            return;
        }
        switch(sig)
        {
            case SIGHUP:
                syslog(LOG_INFO, "Hangup signal caught");
                _pDaemon->GetParser()->ReadConfig(true);
                syslog(LOG_INFO, "Config was updated");
                static_cast<Daemon *>(_pDaemon)->UpdateData();
                syslog(LOG_INFO, "Process is going on...");
            break;
            case SIGTERM:
                syslog(LOG_INFO, "Terminate signal caught");
                unlink(CONSTANTS::PID_PATH.c_str());
                _pDaemon->finish();
            break;
            default:
                syslog(LOG_INFO, "Unknown signal caught");
            break;
        }
    }
};

