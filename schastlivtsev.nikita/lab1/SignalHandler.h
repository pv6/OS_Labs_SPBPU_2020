#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

#include "SettingsManager.h"

typedef void (*signalHandlerFunc)(int);

class SignalHandler
{
    public:
        SignalHandler();
        virtual ~SignalHandler();

        void setSettingsManager(SettingsManager* settingsManager);
        signalHandlerFunc getSigtermHandler();
        signalHandlerFunc getSighupHandler();
        static void removePidFile(char const * const filename);
    protected:
    private:
};

#endif // SIGNALHANDLER_H
