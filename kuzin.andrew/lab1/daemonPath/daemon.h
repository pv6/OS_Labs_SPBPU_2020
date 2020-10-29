#ifndef DAEMON_H
#define DAEMON_H

#include <iostream>
#include <fstream>
#include <csignal>
#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>
#include <list>

#include "DaemonException.h"
#include "Event.h"

class Daemon
{
public:
    void run(int argc, char *argv[]);
    static Daemon& getInstance();
private:
    Daemon() = default;
    Daemon(Daemon const&);
    Daemon& operator= (Daemon const&);

    static const int TIME_SLEEP = 10;
    //static std::string PID_FILE;

    static void makeFork();
    static void signalHandler(int sig);
    static void callSystem(const std::string &eventText);

    static void savePid();
    static void killPreviousDaemon();

    void init(int argc, char *argv[]);

    void initConfig(int argc, char *argv[]);
    void readConfig();

    void doRemind();

    std::string confPath_;
    std::list<Event> listEvent_;
};


#endif // DAEMON_H
