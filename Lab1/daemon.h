#ifndef DAEMON_H
#define DAEMON_H

#include <string>
#include <iostream>
#include <fstream>
#include <csignal>
#include <syslog.h>
#include <unistd.h>
#include <sys/stat.h>
#include <list>
#include <sstream>
#include <iterator>
#include <time.h>

#include "Event.h"

class Daemon
{
public:
    void execute();
    static Daemon* init(int argc,char **argv);
    static void signal_handler(int sig);
private:
    Daemon(const char* cfg_path);
    static void check_args(int argc);
    static void do_fork();
    static void change_dir();
    static void close_streams();
    static Daemon* get_instance(const char*);
    void kill_prev_instance();
    void read_config();
    void print_event_text(const std::string&);
private:
    Daemon(Daemon const&) = delete;
    Daemon& operator= (Daemon const&) = delete;
private:
    static Daemon* instance;
    std::string pid_file;
    std::list<Event> events;
    std::string cfg_path;
    int interval;
};

#endif //DAEMON_H