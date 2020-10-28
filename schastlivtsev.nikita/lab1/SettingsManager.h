#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include "ProcedureBoss.h"
#include "LogLogger.h"
#include "Settings.h"

class SettingsManager
{
    public:
        SettingsManager();
        virtual ~SettingsManager();

        void linkEveryone(ProcedureBoss* procBoss, LogLogger* runnable);
        void updateSettings();
        void setConfig(char const * const filename);
        void finishSignal();
    protected:
    private:
        ProcedureBoss* procedureBoss = nullptr;
        LogLogger* logLogger = nullptr;
        Settings settings;
        char* configName = nullptr;

        // functions
        void assignSettings();
};

#endif // SETTINGSMANAGER_H
