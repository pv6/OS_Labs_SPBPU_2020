#include <iostream>
#include <exception>
#include "DaemonCreator.h"
#include "SettingsManager.h"
#include "LogLogger.h"
#include "ProcedureBoss.h"
#include <syslog.h>

int main(int argc, char const * const argv[])
{
    int success = 0;
    int error = 1;
    try {
        // parse config -> create daemon -> launch routine

        // config parsing
        char const * config = nullptr;
        if ((argc >= 1) && (argv[1])) { // get config out of arguments
            config = argv[1];
        }
        else {
            throw std::runtime_error("Expected config name as the first parameter in CLI");
        }

        SettingsManager settingsManager;  // create settings manager (lazy-init)
        // Daemon creator requires settings manager to create signal handlers

        // daemon creation
        bool parent = DaemonCreator::createDaemon(settingsManager); // make all preparations (like forks)
        if (parent) {
            return success;  // kill parent process
        }

        // Pass config file to parse
        settingsManager.setConfig(config);

        ProcedureBoss procBoss; // create procedure manager who launches procedure periodically
        LogLogger logLogger; // create log logger to log logs (main daemon procedure executor)
        settingsManager.linkEveryone(&procBoss, &logLogger); // link to the settings manager
        settingsManager.updateSettings(); // parse config and set values to start

        // launch routine
        procBoss.launchProcCycle(logLogger); // launch main cycle

    } catch(std::runtime_error& ex) {
        std::string whatMsg = ex.what();
        openlog("logLoggerDaemon", LOG_PID | LOG_NDELAY, LOG_DAEMON);
        std::string errMsg("Exception caught: ");
        errMsg += ex.what();
        syslog(LOG_ERR, errMsg.c_str());
        closelog();
        return error;
    } catch(...) {
        openlog("logLoggerDaemon", LOG_PID | LOG_NDELAY, LOG_DAEMON);
        syslog(LOG_ERR, "Unknown exception caught");
        closelog();
        return error;
    }
    return success;
}
