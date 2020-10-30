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
    if (fork() == -1)
        throw std::runtime_error("Start fork failed");

    // open system log
    openlog(tag.c_str(), LOG_PID | LOG_NDELAY | LOG_PERROR, LOG_LOCAL0);

    // set up signal handling
    signal(SIGTERM, signalHandle);
    signal(SIGHUP, signalHandle);

    try
    {
        int pid, status;
        switch (pid = fork())
        {
        case -1:
            syslog(LOG_ERR, "Initial fork failed");
            return false;
        case 0:
            // Inside child process
            break;
        default:
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
                syslog(LOG_LOCAL0, "Initially-forked process normally exited with code: %i",
                       WEXITSTATUS(status));
            else
                syslog(LOG_ERR, "Some error when exiting initially-forked process");
            // Inside parent process
            return true;
        }

        if (setsid() == -1)
            throw std::runtime_error("setsid fail. errno: " + std::to_string(errno));

        config.load();

        int pid2;
        switch (pid2 = fork())
        {
        case -1:
            throw std::runtime_error("Work process fork failed");
        case 0:
            syslog(LOG_LOCAL0, "Created child work process");
            handlePidFile();
            workLoop();
            syslog(LOG_LOCAL0, "Exit parent process");
            break;
        default:
            waitpid(pid, &status, 0);
            if (WIFEXITED(status))
                syslog(LOG_LOCAL0, "Secondary-forked process normally exited with code: %i",
                       WEXITSTATUS(status));
            else
                syslog(LOG_ERR, "Some error when exiting secondary-forked process");
            break;
        }
    }
    catch (std::exception &exception)
    {
        syslog(LOG_ERR, "Exception caught: %s", exception.what());
        run = false;
        return false;
    }

    closelog();
    return true;
}

DiskMonitor & DiskMonitor::get()
{
    return instance;
}

void DiskMonitor::workLoop()
{
     run = true;
     syslog(LOG_LOCAL0, "Start work loop");
     while (run)
         work();
     syslog(LOG_LOCAL0, "Finish work loop: run flag == false");
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
        pid_ifs.close();

        // find old process
        std::string path_to_old = "/proc/" + std::to_string(pid);

        // send SIGTERM
        if (std::filesystem::exists(path_to_old))
            kill(pid, SIGTERM);
        else
            syslog(LOG_LOCAL0, "First run or incorrect PID");
    }

    std::ofstream pid_ofs(pidFile);
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
            instance.run = false;
        }
        catch (std::exception &exception)
        {
            syslog(LOG_ERR, "%s", exception.what());
        }
        break;
    case SIGTERM:
        syslog(LOG_LOCAL0, "%s", "SIGTERM: finishing DM daemon");
        instance.run = false;
        break;
    default:
        syslog(LOG_LOCAL0, "Unhandled signal catched: %d", sigType);
    }
}
