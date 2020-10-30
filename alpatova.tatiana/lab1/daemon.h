#ifndef LAB1_DAEMON_H
#define LAB1_DAEMON_H
#include <string>
#include <fstream>
#include <syslog.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <csignal>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#define PID_FILE "/var/run/lab1.pid"

enum Mode{
    MODE_0,
    MODE_1,
};

class daemon {
private:
    static std::string config_path;
    static std:: string pid_path;
    static std::string dir1;
    static std::string dir2;
    inline static bool is_finished;
    static size_t interval;

    static void read_config();
    static void delete_prev_daemon();
    static void set_pid_file();
    static bool daemon_fork();
    static void signal_handler(int signal);
    static void work(const std::string& dir1, const std::string& dir2, Mode mode);
    static void move(const char* src_path, const char* dst_path);
public:
    static bool create();
    static void run();
    static void set_config(std::string &conf_file);
    static void finish();
    daemon() = delete;
};


#endif //LAB1_daemon_H
