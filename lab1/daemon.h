//
// Created by Daria on 10/23/2020.
//

#ifndef DAEMON_LAB1_DAEMON_H
#define DAEMON_LAB1_DAEMON_H

#include <string>
#include <set>
#include "error.h"

class daemon {
public:
    static void daemonize();
    static error::error_name init(char* config);
private:
    static std::string dir_first_;
    static std::string dir_second_;
    static std::string pid_file_;
    static std::string dir_home_;
    static std::string hist_log_;
    static char* config_;
    static unsigned int interval_;
    static const std::set<std::string> id_file_;

    static error::error_name load_config();
    static error::error_name set_pid();
    static error::error_name check_pid();
    static void set_signals();
    static void signal_handler(int sig_num);
    static void clear();
    static void print_dir_inner(std::ofstream& hist_log, std::string& path, std::basic_string<char> dir_up);
    static std::string full_path(std::string& path);
    static void print_full_info();
    static void work();
};

#endif //DAEMON_LAB1_DAEMON_H
