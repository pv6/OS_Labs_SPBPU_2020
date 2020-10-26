//
// Created by Daria on 10/23/2020.
//

#include "daemon.h"
#include "parser.h"
#include <cstdlib>
#include <syslog.h>
#include <climits>
#include <unistd.h>
#include <csignal>
#include <sys/stat.h>
#include <map>
#include <cstring>
#include <fstream>
#include <dirent.h>
#include <ctime>
#include "user_exception.h"

char* daemon::config_ = nullptr;
std::string daemon::pid_file_;
std::string daemon::dir_first_;
std::string daemon::dir_second_;
std::string daemon::dir_home_;
std::string daemon::hist_log_;
unsigned int daemon::interval_;
const std::set<std::string> daemon::id_file_ = {"dir_1", "dir_2", "interval"};

error::error_name daemon::init(const std::string &config) {
    error::error_name status = error::OK;
    pid_t pid = fork();
    if (pid == -1) {
        std::string error_s = "First fork failed";
        syslog(LOG_ERR, "%s", error_s.c_str());
        clear();
        throw user_exception(error_s, true);
    }
    if (pid == 0) {
        pid_file_ = "/var/run/lab1.pid";
        openlog((new std::string("DAEMON"))->c_str(), LOG_PID | LOG_NDELAY, LOG_USER);
        config_ = realpath(config.c_str(), nullptr);
        if (config_ == nullptr) {
            status = error::WRONG_FILE;
            syslog(LOG_ERR, "%s", error::print_error(status));
            return status;
        }
        char dir[PATH_MAX];
        getcwd(dir, PATH_MAX);
        dir_home_ = dir;
        syslog(LOG_INFO, "Current directory is %s", dir);
        set_signals();
        status = load_config();
    }
    return status;
}

void daemon::daemonize() {
    setsid();
    pid_t pid = fork();
    if (pid == -1) {
        std::string error_s = "Creating daemon's child failed";
        syslog(LOG_ERR, "%s", error_s.c_str());
        clear();
        throw user_exception(error_s, true);
    }
    if (pid == 0) {
        umask(0);
        if (chdir("/") == -1) {
            std::string error_s = "Chdir failed";
            syslog(LOG_ERR, "%s", error_s.c_str());
            clear();
            throw user_exception(error_s, true);
        }
        work();
    }
}


void daemon::set_signals() {
    std::signal(SIGTERM, signal_handler);
    std::signal(SIGHUP, signal_handler);
}

void daemon::clear() {
    free(config_);
    syslog(LOG_INFO, "Closing and sleeping...");
    closelog();
}

void daemon::signal_handler(int sig_num) {
    switch(sig_num){
        default:
            break;
        case SIGHUP:
            if (load_config() != error::OK) {
                clear();
                std::string error_s = "Impossible to overload config on SIGHUP";
                throw user_exception(error_s, true);
            }
            break;
        case SIGTERM:
            clear();
            std::string error_s = "SIGTERM is received";
            throw user_exception(error_s, true);
    }
}


error::error_name daemon::load_config() {
    error::error_name status;
    std::ifstream file(config_);
    if (!file) {
        status = error::NOT_OPENED;
        syslog(LOG_ERR, "%s", error::print_error(status));
        clear();
        return status;
    }
    auto* parser_ = new parser();
    status = parser::parse_config(file);
    file.close();
    if (status != error::OK) {
        syslog(LOG_ERR, "%s %s", error::print_error(error::PARSER_ERROR), error::print_error(status));
        clear();
        return error::PARSER_ERROR;
    }
    std::map<std::string, std::string> parser_map = parser_->get_map();
    std::set<std::string>::iterator it;
    for (it = id_file_.begin(); it != id_file_.end(); it++) {
        if (parser_map.find(*it) != parser_map.end()) {
            if (*it == "interval") {
                interval_ = std::stoi(parser_map[*it]);
            } else if (*it == "dir_1") {
                dir_first_ = parser_map[*it];
                dir_first_ = full_path(dir_first_);
                if (dir_first_.empty()) {
                    status = error::DIR_ERROR;
                    syslog(LOG_ERR, "%s", error::print_error(status));
                    clear();
                    return status;
                }
            } else {
                dir_second_ = parser_map[*it];
                dir_second_ = full_path(dir_second_);
                if (dir_second_.empty()) {
                    status = error::DIR_ERROR;
                    syslog(LOG_ERR, "%s", error::print_error(status));
                    clear();
                    return status;
                }
            }
        } else {
            status = error::WRONG_CONFIG;
            syslog(LOG_ERR, "%s", error::print_error(status));
            clear();
            return status;
        }
    }
    hist_log_ = dir_second_ + "/hist.log";
    free(parser_);
    return status;
}

std::string daemon::full_path(std::string &path) {
    if (path[0] == '/') {
        return path;
    }
    path = dir_home_ + '/' + path;
    char* real_path = realpath(path.c_str(), nullptr);
    if (real_path == nullptr) {
        syslog(LOG_ERR, "Can't get full path from directory: %s", path.c_str());
        return std::string();
    }
    std::string res(real_path);
    free(real_path);
    return res;
}


error::error_name daemon::set_pid() {
    error::error_name status = error::OK;
    std::ofstream out;
    out.open(pid_file_, std::ofstream::out | std::ofstream::trunc);
    if (!out.is_open()) {
        status = error::PID_ERROR;
        syslog(LOG_ERR, "%s", error::print_error(status));
        return status;
    }
    out << getpid();
    out.close();
    return status;
}

error::error_name daemon::check_pid() {
    std::ifstream pid_file(pid_file_);
    if (pid_file) {
        pid_t current;
        pid_file >> current;
        pid_file.close();
        std::string path_to_daemon = "/proc/" + std::to_string(current);
        struct stat sb{};
        if (stat(path_to_daemon.c_str(), &sb) == 0)
        {
            kill(current, SIGTERM);
        }
    }
    return set_pid();
}

void daemon::print_dir_inner(std::ofstream &hist_log, std::string &path, std::string dir_up) {
    DIR *dir = nullptr;
    struct dirent *entry;
    if (!(dir = opendir(path.c_str()))) {
        syslog(LOG_ERR, "%s", error::print_error(error::DIR_ERROR));
        return;
    }
    while ((entry = readdir (dir)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            if((strcmp( entry->d_name, ".") == 0) ||
                (strcmp( entry->d_name, "..") == 0)) {
                continue;
            }
            std::string new_path = path + "/" + std::string(entry->d_name);
            std::string dir_name;
            if (dir_up.empty())
                dir_name = std::string(entry->d_name);
            else
                dir_name = dir_up + "/" + std::string(entry->d_name);
            hist_log << dir_name << "/" << std::endl;
            print_dir_inner(hist_log, new_path, dir_name);
        } else {
            if (dir_up.empty()) {
                hist_log <<  entry->d_name << "/" << std::endl;
            } else {
                hist_log << dir_up << "/" << entry->d_name << "/" << std::endl;
            }
        }
    }
    closedir(dir);
}

void daemon::print_full_info() {
    std::ofstream hist;
    hist.open(hist_log_, std::ostream::out | std::ostream::app);
    if (hist.is_open()) {
        char output[80];
        time_t seconds = time(nullptr);
        tm* time_info = localtime(&seconds);
        strcpy(output, asctime(time_info));
        hist << output;
        print_dir_inner(hist, dir_first_, std::string());
        hist << std::endl;
        hist.close();
    }
}

void daemon::work() {
    pid_file_ = full_path(pid_file_);
    if (pid_file_.empty() || !check_pid()) {
        clear();
        std::string error_s = "Pid failed";
        throw user_exception(error_s, true);
    }
    syslog(LOG_INFO, "Starting...");
    while (1) {
        print_full_info();
        sleep(interval_);
    }
}