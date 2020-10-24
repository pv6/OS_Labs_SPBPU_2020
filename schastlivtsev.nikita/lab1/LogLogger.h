#ifndef LOGLOGGER_H
#define LOGLOGGER_H

#include <string>
#include "DaemonProcedure.h"

class LogLogger: public DaemonProcedure
{
    public:
        LogLogger();
        virtual ~LogLogger();

        // setters
        void setDirs(char const * const src, char const * const dest);
        void setTotalLogName(char const * const filename = nullptr);

        // main procedure
        virtual void run(); // to log logs

    protected:

    private:
        char* srcDir = nullptr;
        char* destDir = nullptr;
        char* totalLogName = nullptr;

        // constants
        static char constexpr * const totalLogDefault = "total.log";

        // functions
        bool readyLog();
        std::string getTotalLogPath();
};

#endif // LOGLOGGER_H
