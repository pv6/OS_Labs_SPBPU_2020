#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include "Daemon.h"
#include "Parser.h"

std::string Daemon::pid_file_path = "/var/run/daemon_pid.pid";
std::vector<std::string> Daemon::folds;
std::string Daemon::config_file;
bool Daemon::proceed = true;
char Daemon::dir_home_path [PATH_LENGTH];

void Daemon::set_config(const char* FileName) {
    if (getcwd(dir_home_path, PATH_LENGTH) == nullptr) {
        throw std::runtime_error("Couldn't open current directory");
    }
    config_file = std::string(dir_home_path) + "/" + FileName;
    folds = Parser::parse(config_file);
    proceed = true;
}

void Daemon::set_pid() {
    std::ofstream pid_file(pid_file_path.c_str());
    if (!pid_file.is_open()) {
        pid_file << getpid();
        pid_file.close();
    }
}

void Daemon::kill_daemon() {
    std::ifstream pid_file(pid_file_path.c_str());

    if (pid_file.is_open() && !pid_file.eof()) {
        pid_t prev_pid;
        pid_file >> prev_pid;

        if (prev_pid > 0) {
            kill(prev_pid, SIGTERM);
        }
    }
    pid_file.close();
}

void Daemon::signal_handler(int sig) {
    if (sig == SIGHUP) {
        Parser::parse(config_file);
        if (folds.size() != 2) {
            syslog(LOG_ERR, "Can't read config file");
            proceed = false;
        }
        else {
            syslog(LOG_NOTICE, "Rereading config");
        }
    }
    else if (sig == SIGTERM) {
        syslog(LOG_NOTICE, "Terminating daemon");
        proceed = false;
    }
}

void Daemon::copy_file(const std::string& src_path, const std::string& dest_path) {
    std::ifstream from(src_path.c_str(), std::ios::binary);
    std::ofstream to(dest_path.c_str(), std::ios::binary);
    to << from.rdbuf();
}

bool Daemon::delete_directory(const std::string& dir_path) {
    DIR* dir;
    dir = opendir(dir_path.c_str());
    if (!dir) {
        syslog(LOG_ERR, "Could not open directory: %s", dir_path.c_str());
        return false;
    }
    struct dirent *ent;
    while ((ent = readdir(dir)) != nullptr) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        if (ent->d_type == DT_DIR) {
            if (!delete_directory(ent->d_name)) {
                return false;
            }
        }
        if (unlink((dir_path + ent->d_name).c_str()) == -1) {
            syslog(LOG_NOTICE, "%s file wasn't deleted", ent->d_name);
            return false;
        }
    }
    return true;
}

bool Daemon::work() {
    std::string dir1_path = std::string(dir_home_path) + "/" + folds[0];
    std::string dir2_path = std::string(dir_home_path) + "/" + folds[1];
    DIR* dir;
    dir = opendir(dir1_path.c_str());
    if (!dir) {
        syslog(LOG_ERR, "Could not open directory: %s", folds[0].c_str());
        return false;
    }
    if (!delete_directory(dir2_path)) {
        syslog(LOG_ERR, "Can't clean directory");
        return false;
    }
    struct dirent *ent;
    while ((ent = readdir(dir)) != nullptr) {
        std::string::size_type idx;
        std::string ent_path = std::string(ent->d_name);
        idx = ent_path.rfind('.');
        if (idx != std::string::npos) {
            std::string extension = ent_path.substr(idx + 1);
            if (extension == std::string("bk")) {
                copy_file(dir1_path + "/" + ent_path, dir2_path + "/" + ent_path);
            }
        }
    }
    closedir(dir);
    return true;
}

void Daemon::run() {
    while(proceed && work()) {
        syslog(LOG_NOTICE, "Working...");
        sleep (WaitTime);
    }
}

bool Daemon::init(const char* FileName) {
    set_config(FileName);
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "Unable to create first fork");
        return false;
    }

    if (pid > 0) {
        syslog(LOG_NOTICE, "Create first fork");
        return false;
    }

    if (setsid() < 0) {
        syslog(LOG_ERR, "Unable to setid");
        return false;
    }

    syslog(LOG_NOTICE, "First fork done");
    pid = fork();

    if (pid < 0) {
        syslog(LOG_ERR, "Unable to create second fork");
        return false;
    }
    if (pid > 0) {
        syslog(LOG_NOTICE, "Parent process done");
        return false;
    }

    signal(SIGHUP, Daemon::signal_handler);
    signal(SIGTERM, Daemon::signal_handler);

    umask(0);
    chdir("/");
    kill_daemon();
    set_pid();
    syslog(LOG_NOTICE, "Second fork done");
    return true;
}


