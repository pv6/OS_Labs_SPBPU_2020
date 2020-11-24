#include "DirectoryCleaner.hpp"

#include <csignal>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <syslog.h>
#include <sys/stat.h>
#include <unistd.h>

using std::exception;
using std::ofstream;
using std::string;
using std::to_string;
using std::ifstream;
namespace fs = std::filesystem;

namespace
{
    const string paramName{"DIR_1="};

    string readConfig(const string& path)
    {
        ifstream confFile(path);

        if (!confFile.is_open())
        {
            syslog(LOG_ERR, "Could not open config file: %s", path.c_str());
            throw exception();
        }

        string lineBuf, param;
        int    delimiterPos;
        while (getline(confFile, lineBuf))
        {
            delimiterPos = lineBuf.find(paramName);
            if (delimiterPos != string::npos)
            {
                param = lineBuf.substr(delimiterPos + paramName.length());
            }
            else
            {
                syslog(LOG_WARNING,
                       "Config string (%s) did not contain parameter name (%s)",
                       lineBuf.c_str(),
                       paramName.c_str());
                continue;
            }
        }

        if (param.empty())
        {
            syslog(LOG_ERR,
                   "Could not find param (%s) in config (%s)",
                   paramName.c_str(),
                   path.c_str());
            throw exception();
        }

        if (!fs::exists(param))
        {
            syslog(LOG_ERR,
                   "Param value (%s) in config (%s) is not a directory",
                   param.c_str(),
                   path.c_str());
            throw exception();
        }

        return param;
    }
}

DirectoryCleaner::DirectoryCleaner()
{
    openlog(loggingTag.c_str(), LOG_PID | LOG_PERROR, LOG_DAEMON);
}

DirectoryCleaner::~DirectoryCleaner()
{
    closelog();
}

DirectoryCleaner& DirectoryCleaner::instance()
{
    static DirectoryCleaner instance;
    return instance;
}

DirectoryCleaner& DirectoryCleaner::instance(const string& configPath)
{
    instance().configPath = configPath;
    instance().dir        = readConfig(configPath);
    return instance();
}

void DirectoryCleaner::reloadConfig(int signalId)
{
    try
    {
        syslog(LOG_INFO, "Reloading config: %s", instance().configPath.c_str());
        instance().dir = readConfig(instance().configPath);
    }
    catch(const std::exception& e)
    {
        syslog(LOG_ERR,
               "Could not reload config: %s",
               instance().configPath.c_str());
    }
}

void DirectoryCleaner::terminateExecution(int signalId)
{
    syslog(LOG_INFO, "Terminating execution");
    instance().isRunning = false;
}

void DirectoryCleaner::run()
{
    switch (fork())
    {
        case -1:
        {
            syslog(LOG_ERR,
                   "Could not daemonize – first fork() failed with error: %d",
                   errno);
            throw exception();
        }
        case 0:
        {
            syslog(LOG_INFO, "Entered child process during first fork");
            break;
        } 
        default:
        {
            syslog(LOG_INFO,
                   "Entered parent process during first fork. Terminating");
            return;
        }
    }

    if (setsid() < 0)
    {
        syslog(LOG_ERR,
               "Could not create new session – setsid() failed with error: %d",
               errno);
        throw exception();
    }

    switch (fork())
    {
        case -1:
        {
            syslog(LOG_ERR,
                   "Could not daemonize – second fork() failed with error: %d",
                   errno);
            throw exception();
        }
        case 0:
        {
            syslog(LOG_INFO, "Entered child process during second fork");
            break;
        }
        default:
        {
            syslog(LOG_INFO,
                   "Entered parent process during second fork. Terminating");
            return;
        }
    }
        for (int descriptor = getdtablesize(); descriptor >= 0; --descriptor)
        {
                close(descriptor);
        }

        int standardDescriptor = open("/dev/null", O_RDWR); //stdin
        dup(standardDescriptor); //stdout
        dup(standardDescriptor); //stderr

        signal(SIGHUP, reloadConfig);
        signal(SIGTERM, terminateExecution);

        umask(0);

        ensureUnqnessViaPid();
        syslog(LOG_INFO, "Successfully daemonized the process. Starting work");
        isRunning = true;
        while (isRunning)
        {
            rmvTmpFilesInDir();
            sleep(scndsBtwnExecutions);
        }
}

void DirectoryCleaner::rmvTmpFilesInDir()
{
    try
    {
        for (const auto& entry : fs::directory_iterator(instance().dir))
        {
            if (!!entry.path().string().find(".tmp"))
            {
                syslog(LOG_ERR,
                       "Removing file: %s",
                       entry.path().string().c_str());
                fs::remove(entry.path());
            }
        }
    }
    catch(const fs::filesystem_error& e)
    {
        syslog(LOG_ERR, "Could not open directory: %s", instance().dir.c_str());
        throw exception();
    }
}

void DirectoryCleaner::ensureUnqnessViaPid()
{
    ifstream pidFileIn(pidFilePath);
    if (pidFileIn.is_open())
    {
        pid_t pid;
        pidFileIn >> pid;

        string oldProcPath = "/proc/" + to_string(pid);
        if (fs::exists(oldProcPath))
        {
            syslog(LOG_INFO,
                   "Found existing process (pid: %d), terminating it",
                   pid);
            kill(pid, SIGTERM);
        }
        pidFileIn.close();
    }

    ofstream pidFileOut(pidFilePath);
    if (!pidFileOut.is_open())
    {
        syslog(LOG_ERR,
               "Could not create/open pid file (%s) for writing",
               pidFilePath.c_str());
        throw exception();
    }

    pid_t pid = getpid();
    pidFileOut << pid << std::endl;
    pidFileOut.close();
    syslog(LOG_INFO,
           "Successfully wrote pid (%d) to file (%s)",
           pid,
           pidFilePath.c_str());
}
