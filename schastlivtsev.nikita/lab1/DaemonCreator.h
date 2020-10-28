#ifndef DAEMONCREATOR_H
#define DAEMONCREATOR_H

#include <sys/types.h>
#include "SettingsManager.h"

class DaemonCreator
{
    public:
        // creation procedure
        static bool createDaemon(SettingsManager& settingsManager);
    protected:

    private:
        // functions
        static inline bool forkWrapper();
        static void savePid(char const * const pidFile, const pid_t pid);
        static void killBrother(char const * const pidFile);

        // deprecated
        DaemonCreator() = delete;
        virtual ~DaemonCreator() = delete;
};

#endif // DAEMONCREATOR_H
