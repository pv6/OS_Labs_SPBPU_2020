#ifndef LAB1_DAEMON_H
#define LAB1_DAEMON_H

#include<string>
#include<vector>
#include"Parser.h"
//#include <stdio>
//#include <stdlib>
//#include <unistd.h>
#include <csignal>
#include <syslog.h>

class Daemon {
private:
    static bool init();
    static void signal_handler(int sig);
    static void copy_file(std::string src_path, std::string dest_path);
    static bool delete_directory(const std::string& dir_path);
    static bool work();
    static void kill_daemon();
    static void set_pid();

private:
    static std::vector<std::string> folds;
    static std::string config_file;
    static const int WaitTime = 30;
    static const size_t PATH_LENGTH = 100;
    static bool proceed;
    static char dir_home_path [PATH_LENGTH];

public:
    explicit Daemon(const char* FileName);
    static void run();
    ~Daemon() = default;


};

#endif //LAB1_DAEMON_H