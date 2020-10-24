#include "SignalHandler.h"
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <fstream>

SignalHandler::SignalHandler()
{
    //ctor
}

SignalHandler::~SignalHandler()
{
    //dtor
}

namespace {
    static SettingsManager* s_settingsManager = nullptr;

    static void sigTermHandler(int) {
        syslog(LOG_INFO, "SIGTERM caught, exiting");
        SignalHandler::removePidFile(s_settingsManager->getPidFile());
        exit(0);
    }

    static void sigHupHandler(int) {
        if (!s_settingsManager)
            throw std::runtime_error("Can't handle SIGHUP without SettingsManager");
        syslog(LOG_INFO, "SIGHUP caught, updating settings");
        s_settingsManager->updateSettings();
    }

};


void SignalHandler::setSettingsManager(SettingsManager* settingsManager) {
    s_settingsManager = settingsManager;
}


signalHandlerFunc SignalHandler::getSigtermHandler() {
    return sigTermHandler;
}


signalHandlerFunc SignalHandler::getSighupHandler() {
    return sigHupHandler;
}


void SignalHandler::removePidFile(char const * const filename) {
    std::ifstream checkExist(filename, std::ios::in);
    if (checkExist.good()) {
        pid_t pid;
        checkExist >> pid;
        checkExist.close();
        if (pid == getpid()) {  // remove only pid of myself
            std::remove(filename);
        }
    }
}
