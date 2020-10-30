#include <signal.h>
#include <sys/syslog.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>
#include "config.h"
#include "diskmonitor.h"

DiskMonitor DiskMonitor::instance;

void DiskMonitor::init(const std::string &configName)
{
    config.setName(configName);
}

bool DiskMonitor::start()
{
    // open system log
    openlog(tag.c_str(), LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_LOCAL0);

    // set up signal handling
    signal(SIGTERM, signalHandle);
    signal(SIGHUP, signalHandle);

    try
    {
        switch (fork())
        {
        case -1:
            syslog(LOG_ERR, "Initial fork failed");
            return false;
        case 0:
            // Inside child process
            break;
        default:
            // Inside parent process
            return true;
        }

        config.load();

        switch (fork())
        {
        case -1:
            throw std::runtime_error("Work process fork failed");
        case 0:
            syslog(LOG_INFO, "Created child work process");
            handlePidFile();
            workLoop();
            syslog(LOG_INFO, "Exit parent process");
            break;
        default:
            break;
        }

        if (setsid() == -1)
            throw std::runtime_error("setsid fail. errno: " + std::to_string(errno));
    }
    catch (std::exception &exception)
    {
        syslog(LOG_ERR, "Exception caught: %s", exception.what());
        run = false;
        return false;
    }

    return true;
}

DiskMonitor & DiskMonitor::get()
{
    return instance;
}

void DiskMonitor::workLoop()
{
     run = true;
     syslog(LOG_INFO, "Start work loop");
     while (run)
         work();
     syslog(LOG_INFO, "Finish work loop: run flag == false");
}

void DiskMonitor::work()
{
    std::cout << "I am alive!\n";
    sleep(1);
}

void DiskMonitor::handlePidFile()
{
    std::ifstream pid_ifs(pidFile);

    if (pid_ifs)
    {
        pid_t pid;
        pid_ifs >> pid;

        // find old process
        std::string path_to_old = "/proc/" + std::to_string(pid);

        // send SIGTERM
        if (std::filesystem::exists(path_to_old))
            kill(pid, SIGTERM);
        else
            syslog(LOG_INFO, "First run or incorrect PID");
    }

    std::ofstream pid_ofs;
    pid_ofs.open(pidFile, std::ofstream::out | std::ofstream::trunc);
    if (!pid_ofs)
        throw std::runtime_error("Could not open PID file for writing");
    pid_ofs << getpid() << "\n";
    pid_ofs.close();
}

void DiskMonitor::signalHandle(int sigType)
{
    switch (sigType)
    {
    case SIGHUP:
        try
        {
            instance.config.load();
            syslog(LOG_INFO, "Config reload");
        }
        catch (std::exception &exception)
        {
            syslog(LOG_ERR, "%s", exception.what());
        }
        break;
    case SIGTERM:
        syslog(LOG_INFO, "%s", "SIGTERM: finishing DM daemon");
        instance.run = false;
        break;
    default:
        syslog(LOG_INFO, "Unhandled signal catched: %d", sigType);
    }
}
