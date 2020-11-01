#include "../include/DaemonManager.h"

#include <unistd.h>
#include <syslog.h>
#include <sys/stat.h>
#include <signal.h>
#include <stdlib.h>
#include <linux/limits.h>

#include <fstream>
#include <iostream>
#include <string>



int DaemonManager::_wait_time = 0;

ConfigParser DaemonManager::_cp;
FileTransfer DaemonManager::_ft;

bool DaemonManager::_status = false;



bool DaemonManager::create_DaemonManager(std::string pid_path, std::string config_path) {
    openlog("lab_1", LOG_NDELAY | LOG_PID, LOG_USER);
    syslog(LOG_DEBUG, "DaemonManager: Initialization start");

    char abs_path[PATH_MAX];
    realpath(config_path.c_str(), abs_path);
    config_path = abs_path;
    
    pid_t pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "DaemonManager: Fork failed");
        closelog();
        return false;
    } else if (pid > 0) {
        syslog(LOG_DEBUG, "DaemonManager: Parent process");
        closelog();
        return false;
    }
    syslog(LOG_DEBUG, "DaemonManager: First fork succeeded");
    
    if (setsid() == -1) {
        syslog(LOG_ERR, "DaemonManager: Setsid failed");
        closelog();
        return false;
    }
    syslog(LOG_DEBUG, "DaemonManager: Setsid succeeded");

    pid = fork();
    if (pid < 0) {
        syslog(LOG_ERR, "DaemonManager: Second fork failed");
        closelog();
        return false;
    } else if (pid > 0) {
        syslog(LOG_DEBUG, "DaemonManager: Second parent process");
        closelog();
        return false;
    }
    syslog(LOG_DEBUG, "DaemonManager: Second fork succeeded");

    umask(0);
    if (chdir("/") == -1) {
        syslog(LOG_ERR, "DaemonManager: Chdir failed");
        closelog();
        return false;
    }
    syslog(LOG_DEBUG, "DaemonManager: Chdir succeeded");

    if (close(STDIN_FILENO) == -1 || close(STDOUT_FILENO) == -1 || close(STDERR_FILENO) == -1) {
        syslog(LOG_ERR, "DaemonManager: Close failed");
        closelog();
        return false;        
    }
    syslog(LOG_DEBUG, "DaemonManager: Close succeeded");

    signal(SIGHUP, DaemonManager::_signal_handler);
    signal(SIGTERM, DaemonManager::_signal_handler);

    syslog(LOG_DEBUG, "DaemonManager: Check existence of pid-file");
    std::ifstream pid_file_in(pid_path);
    if (pid_file_in.is_open()) {
        if (!(pid_file_in.peek() == std::ifstream::traits_type::eof())) {
            int pid_old;
            pid_file_in >> pid_old;

            if (kill(pid_old, 0) == 0) {
                syslog(LOG_DEBUG, "DaemonManager: Process already exists, killing it");
                kill(pid_old, SIGTERM);
            }
        }

        pid_file_in.close();
    } else {
        syslog(LOG_ERR, "DaemonManager: Opening pid-file failed");
        closelog();
        return false;  
    }

    std::ofstream pid_file_out(pid_path);
    if (pid_file_out.is_open()) {
        pid_file_out << getpid();
        syslog(LOG_DEBUG, "DaemonManager: Writing to pid-file succeeded");
        pid_file_out.close();
    } else {
        syslog(LOG_ERR, "DaemonManager: Opening pid-file failed");
        closelog();
        return false;  
    }

    _cp = ConfigParser(config_path);
    if (_update_config() == false) {
        closelog();
        return false; 
    }

    //_pid_path = pid_path;
    //_config_path = config_path;
    _status = true;
    syslog(LOG_DEBUG, "DaemonManager: Initialization end");

    return true;
}
void DaemonManager::run() {
    syslog(LOG_DEBUG, "DaemonManager: Run start");

    while (_status) {
        sleep(_wait_time);
        if (_ft.execute() == false) {
            if (_status == true) {
                syslog(LOG_ERR, "FileTransfer: Execution error");
                _status = false;
            }
        }
    }
}
void DaemonManager::_signal_handler(int signal) {
    switch (signal) {
        case SIGHUP:
            syslog(LOG_DEBUG, "DaemonManager: Config info update request");
            if (_update_config() == false) {
                _status = false;
                closelog();
            }
            break;
        case SIGTERM:
            syslog(LOG_DEBUG, "DaemonManager: Termination request");
            _status = false;
            closelog();
            break;
        default:
            break;
    }
}
bool DaemonManager::_update_config() {
    std::vector<std::string> values = _cp.read_config();
    if (values.size() != 3) {
        syslog(LOG_ERR, "ConfigParser: Parsing config-file failed");
        return false;   
    }
    int time_bufer = std::stoi(values[2]);
    if (values[0][values[0].length() - 1] == '/') {
        values[0].resize(values[0].length() - 1);
    }
    if (values[1][values[1].length() - 1] == '/') {
        values[1].resize(values[1].length() - 1);
    }

    if (values[0].empty() || values[0].empty()) {
        syslog(LOG_ERR, "ConfigParser: Empty filename");
        return false;
    } else if (time_bufer <= 0) {
        syslog(LOG_ERR, "ConfigParser: Negative waittime");
        return false;
    }

    _ft.set_values_from_config(values[0], values[1]);
    _wait_time = time_bufer;
    syslog(LOG_DEBUG, "DaemonManager: Config info has been updated");

    return true;
}