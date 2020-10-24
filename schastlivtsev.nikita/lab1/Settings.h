#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

class Settings
{
    public:
        Settings();
        virtual ~Settings();

        void parseConfig(char const * const filename = defaultConfig);

        // getters
        char const * const getPidPath();
        char const * const getFolderSrc();
        char const * const getFolderDest();
        char const * const getTotalLogName();
        size_t getWaitSec();

    protected:

    private:
        char* pidPath = nullptr;
        char* folderSrc = nullptr;
        char* folderDest = nullptr;
        char* totalLogName = nullptr;
        size_t waitSec = defaultWaitSec;

        // functions
        void checkCriticalFields();

        // constants
        static char constexpr * const defaultPidPath = "/var/totalLog/";
        static char constexpr * const defaultConfig = "config";
        static const size_t defaultWaitSec = 45; // 45 seconds
        static char constexpr * const defaultTotalLog = "total.log";
};

#endif // SETTINGS_H
