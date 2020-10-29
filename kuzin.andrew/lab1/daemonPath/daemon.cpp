#include "daemon.h"

#define PID_FILE "/var/run/daemon.pid"

Daemon& Daemon::getInstance()
{
    static Daemon inst;
    return inst;
}

void Daemon::makeFork()
{
    pid_t pid = fork();
    if (pid == -1)
    {
        std::string err = "Fork failed. Stopping disk_monitor.";
        syslog(LOG_ERR, err.c_str(), "-Wformat-security");
        throw DaemonException(err, ERROR_FATAL);
    }
    else if (pid > 0)
    {
        std::string err = "Successfully made fork.";
        syslog(LOG_NOTICE, "Successfully made fork. Child's pid is %d.", pid);
        throw DaemonException(err, ERROR_NOTICE);
    }
}

void Daemon::savePid()
{
    std::ofstream filePid(PID_FILE);
    if (filePid.is_open())
    {
        filePid << getpid();
        filePid.close();
    }

}
void Daemon::signalHandler(int sig)
{
    std::string err;
    switch(sig)
    {
        case SIGHUP:
            getInstance().readConfig();
            syslog(LOG_NOTICE, "Hangup Signal");
            break;
        case SIGTERM:
            err = "Terminate Signal";
            syslog(LOG_NOTICE, err.c_str(), "-Wformat-security");
            unlink(PID_FILE);
            throw DaemonException(err, ERROR_NOTICE);
        default:
            syslog(LOG_NOTICE, "Unknown Signal");
            break;
    }


}

void Daemon::killPreviousDaemon()
{
    std::ifstream filePid(PID_FILE);
    if (filePid.is_open() && !filePid.eof())
    {
        pid_t prev;
        filePid >> prev;
        if (prev > 0)
        {
            kill(prev, SIGTERM);
        }
    }
    filePid.close();
}

void Daemon::initConfig(int argc, char **argv)
{
    if (argc < 2)
    {
        std::string err = "Incorrect number of arguments.";
        syslog(LOG_ERR, "Incorrect number of arguments.\nExpected: 2.\nGot: %d\n", argc);
        throw DaemonException(err, true);
    }
    confPath_ = std::string(realpath(argv[1], nullptr));
}

void Daemon::readConfig()
{
    std::string eventDate;
    std::string eventTime;
    std::string eventFlag;
    std::string eventText;

    std::ifstream fileConfig(confPath_);
    if (!fileConfig.is_open() || fileConfig.eof())
    {
        std::string err = "Empty file or couldn`t open";
        syslog(LOG_ERR, err.c_str(), "-Wformat-security");
        throw DaemonException(err, ERROR_FATAL);
    }

    listEvent_.clear();
    while ((fileConfig >> eventDate >> eventTime >> eventFlag) && getline(fileConfig, eventText))
    {
        tm time{};

        sscanf(eventDate.c_str(),"%d.%d.%d", &time.tm_mday, &time.tm_mon, &time.tm_year);
        sscanf(eventTime.c_str(),"%d:%d:%d", &time.tm_hour, &time.tm_min, &time.tm_sec);

        time.tm_year -= 1900;
        time.tm_mon -= 1;

        Event event(eventText, eventFlag, time);
        listEvent_.push_back(event);
    }
}

void Daemon::init(int argc, char **argv)
{
    makeFork();

    if (setsid() < 0)
    {
        std::string err = "Couldn't generate session ID for child process.";
        syslog(LOG_ERR, err.c_str(), "-Wformat-security");
        throw DaemonException(err, ERROR_FATAL);
    }

    makeFork();

    umask(0);

    initConfig(argc, argv);

    if ((chdir("/")) < 0)
    {
        std::string err = "Couldn't change working directory to /.";
        syslog(LOG_ERR, err.c_str(), "-Wformat-security");
        throw DaemonException(err, ERROR_FATAL);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);
}

void Daemon::callSystem(const std::string &eventText)
{
    system(std::string(
            ("gnome-terminal --working-directory='/home' -- sh -c \"echo ") + eventText +
            std::string("; read line\"")).c_str());
}

void Daemon::doRemind()
{
    time_t nowTime;
    tm *timeInfo;
    time(&nowTime);
    timeInfo = localtime(&nowTime);

    for (Event event : listEvent_)
    {
        tm timeTmp = event.getEventTime();
        time_t eventTime = mktime(&timeTmp);

        if (nowTime < eventTime)
            continue;

        if (std::abs(timeInfo->tm_sec - timeTmp.tm_sec) <= TIME_SLEEP)
        {
            if (event.getEventFlag().compare("-m") == 0)
            {
                callSystem(event.getEventText());
            } else if (event.getEventFlag().compare("-h") == 0)
            {
                if (timeInfo->tm_min == timeTmp.tm_min)
                {
                    callSystem(event.getEventText());
                }
            } else if (event.getEventFlag().compare("-d") == 0)
            {
                if (timeInfo->tm_hour == timeTmp.tm_hour && timeInfo->tm_min == timeTmp.tm_min)
                {
                    callSystem(event.getEventText());
                }
            } else if (event.getEventFlag().compare("-w") == 0)
            {
                if (timeInfo->tm_wday == timeTmp.tm_wday && timeInfo->tm_hour == timeTmp.tm_hour &&
                    timeInfo->tm_min == timeTmp.tm_min)
                {
                    callSystem(event.getEventText());
                }
            }
        }
    }
}

void Daemon::run(int argc, char *argv[])
{
    openlog("Reminder", LOG_NOWAIT || LOG_PID, LOG_LOCAL0);
    syslog(LOG_NOTICE, "Started reminder");

    init(argc, argv);

    killPreviousDaemon();
    savePid();

    readConfig();

    while (true)
    {
        doRemind();

        sleep(TIME_SLEEP);
    }
}


