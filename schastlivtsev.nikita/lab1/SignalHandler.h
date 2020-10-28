#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

#include "SettingsManager.h"

typedef void (*signalHandlerFunc)(int);

class SignalHandler // helper
{
    public:
        static void setSettingsManager(SettingsManager* settingsManager);
        static signalHandlerFunc getSigtermHandler();
        static signalHandlerFunc getSighupHandler();
        static void removePidFile(char const * const filename);
    protected:
    private:
        SignalHandler() = delete; // it's helper, no instance needed
        virtual ~SignalHandler() = delete;
};

#endif // SIGNALHANDLER_H
