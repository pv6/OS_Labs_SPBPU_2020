#include <signal.h>
#include <sys/stat.h>
#include <sys/syslog.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/inotify.h>
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

    umask(027);

    // close all owned file desrciptors
    for (int i = getdtablesize(); i >= 0; --i)
        close(i);
    int i = open("/dev/null", O_RDWR);
    dup(i); //in
    dup(i); //out

    syslog(LOG_INFO, "Closed all file descriptors before starting");

    if ((inotifyDescr = inotify_init()) < 0)
    {
        syslog(LOG_ERR, "Could not initialize inotify");
        return false;
    }
    syslog(LOG_INFO, "Created inotify descriptor");


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
            syslog(LOG_INFO, "Exit parent 0 process");
            return true;
        }

        switch (fork())
        {
        case -1:
            fail(std::runtime_error("Work process fork failed"));
            break;
        case 0:
            syslog(LOG_INFO, "Created child work process");
            handlePidFile();
            runAll();
            workLoop();
            syslog(LOG_INFO, "Exit main process");
            break;
        default:
            syslog(LOG_INFO, "Exit parent 1 process");
            break;
        }

        if (setsid() == -1)
            fail(std::runtime_error("setsid fail. errno: " + std::to_string(errno)));
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

void DiskMonitor::runAll()
{
    // clear previous watches
    removeWatches();
    config.load();
    eventsCount = config.getMaxEvents();
    syslog(LOG_ERR, "inotify descriptor: %i", inotifyDescr);
    syslog(LOG_ERR, "Got max events: %li", eventsCount);
    watchDirectories = config.getDirectories();
    buildWatchDescrToPath();
}

void DiskMonitor::workLoop()
{
     run = true;
     const unsigned int maxFilePathLength = 16;
     const unsigned int eventSize = sizeof(inotify_event);
     bufferSize = eventsCount * (eventSize + maxFilePathLength);
     syslog(LOG_ERR, "Event buffer size: %li", bufferSize);
     delete []events;
     events = new char[bufferSize];
     if (!events)
         fail(std::runtime_error("No memory for event buffer"));

     syslog(LOG_INFO, "Start work loop");
     while (run)
     {
         work();
     }
     syslog(LOG_INFO, "Finish work loop: run flag == false");
     delete []events;
     events = nullptr;
}

void DiskMonitor::work()
{
    ssize_t length = read(inotifyDescr, events, bufferSize);
    syslog(LOG_ERR, "Read from inotify returned %li", length);
    if (length < 0)
        run = false;

    inotify_event *event = reinterpret_cast<inotify_event *>(events);
    const unsigned int eventSize = sizeof(inotify_event);
    for (int i = 0; i < length; i += eventSize + event->len, event++)
    {
        if (event->len)
        {
            if (event->mask & IN_CREATE)
            {
                if (event->mask & IN_ISDIR)
                    syslog(LOG_INFO, "%s DIR::%s CREATED\n",
                           watchDescrToPath[event->wd].c_str(), event->name);
                else
                    syslog(LOG_INFO, "%s FILE::%s CREATED\n",
                           watchDescrToPath[event->wd].c_str(), event->name);
            }

            if (event->mask & IN_MODIFY)
            {
                if (event->mask & IN_ISDIR)
                    syslog(LOG_INFO, "%s DIR::%s MODIFIED\n",
                           watchDescrToPath[event->wd].c_str(), event->name);
                else
                    syslog(LOG_INFO, "%s FILE::%s MODIFIED\n",
                           watchDescrToPath[event->wd].c_str(), event->name);
            }

            if (event->mask & IN_DELETE)
            {
                if (event->mask & IN_ISDIR)
                    syslog(LOG_INFO, "%s DIR::%s DELETED\n",
                           watchDescrToPath[event->wd].c_str(), event->name);
                else
                    syslog(LOG_INFO, "%s FILE::%s DELETED\n",
                           watchDescrToPath[event->wd].c_str(), event->name);
            }
        }
    }
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
        fail(std::runtime_error("Could not open PID file for writing"));
    pid_ofs << getpid() << "\n";
    pid_ofs.close();
}

void __attribute__((noreturn)) DiskMonitor::fail(std::exception &&exc)
{
    delete []events;
    events = nullptr;
    throw std::move(exc);
}

void DiskMonitor::buildWatchDescrToPath()
{
    // add new watches
    for (auto &path : watchDirectories)
    {
        int wd = inotify_add_watch(inotifyDescr, path.c_str(), IN_CREATE | IN_MODIFY | IN_DELETE);

        if (wd == -1)
            syslog(LOG_ERR, "Could not create watch on %s", path.c_str());
        else
        {
            watchDescrToPath[wd] = path;
        }
    }
}

void DiskMonitor::removeWatches()
{
    for (auto &watch : watchDescrToPath)
    {
        inotify_rm_watch(inotifyDescr, watch.first);
        syslog(LOG_INFO, "Remove watch on %s", watch.second.c_str());
    }
    watchDescrToPath.clear();
}

void DiskMonitor::signalHandle(int sigType)
{
    switch (sigType)
    {
    case SIGHUP:
        try
        {
            syslog(LOG_INFO, "Config reload");
            instance.runAll();
            instance.workLoop();
        }
        catch (std::exception &exception)
        {
            syslog(LOG_ERR, "Exceprion caught: %s", exception.what());
            instance.run = false;
            close(instance.inotifyDescr);
            syslog(LOG_INFO, "Inotify descriptor closed");
        }
        break;
    case SIGTERM:
        syslog(LOG_INFO, "%s", "SIGTERM: finishing DM daemon");
        instance.run = false;
        close(instance.inotifyDescr);
        syslog(LOG_INFO, "Inotify descriptor closed");
        break;
    default:
        syslog(LOG_INFO, "Unhandled signal catched: %d", sigType);
    }
}
