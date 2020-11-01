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
    static void set_config(const char *FileName);
    static void signal_handler(int sig);
    static void copy_file(const std::string& src_path, const std::string& dest_path);
    static bool delete_directory(const std::string& dir_path);
    static bool work();
    static void kill_daemon();
    static void set_pid();

    static std::string pid_file_path;
    static std::vector<std::string> folds;
    static std::string config_file;
    static const int WaitTime = 30;
    static const size_t PATH_LENGTH = 100;
    static bool proceed;
    static char dir_home_path [PATH_LENGTH];

public:
    static bool init(const char* FileName);
    static void run();
    ~Daemon() = default;


};

#endif //LAB1_DAEMON_H