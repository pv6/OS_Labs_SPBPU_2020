#include "../include/daemon09.h"
#include "../include/exceptions.h"
#include <fstream>    // ifstream, ofstream
#include <unistd.h>   // usleep
#include <syslog.h>   // syslog
#include <sys/stat.h> // stat
#include <stdlib.h>   // realpath, system

Daemon09::Daemon09(std::string const& config_path, std::string const& pid_path) :
    Daemon(config_path, pid_path) {
    configure();
}

// this func will throw exceptions, reconfigure() will not
void Daemon09::configure() {
    // open config file
    std::ifstream config_ifstream;
    config_ifstream.open(m_config_path);
    if (!config_ifstream.good()) {
        throw OpenFileException();
    }

    // read config file
    std::string config;
    std::string value;
    while (config_ifstream >> config) {
        if (!(config_ifstream >> value)) {
            throw InvalidConfigException();
        }
        if (std::string("DIR_FROM").compare(config) == 0) {
            m_dir_from = value;
        } else if (std::string("DIR_TO").compare(config) == 0) {
            m_dir_to = value;
        } else if (std::string("INTERVAL").compare(config) == 0) {
            // exception may be caught by main
            m_interval_s = std::stoi(value);
        } else {
            throw InvalidConfigException();
        }
    }

    // check dirs whether exist or not (try to create if not)
    struct stat info;
    if ((stat(m_dir_from.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) ||
        (stat(m_dir_from.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR))) {
        std::string command = "mkdir -p ";
        if (system((command + m_dir_from).c_str()) != 0 ||
            system((command + m_dir_to).c_str()) != 0) {
            throw InvalidPathException();
        }
    }

    // check specified interval on validity
    if (m_interval_s <= 0) {
        throw InvalidIntervalException();
    }

    syslog(LOG_INFO, "configuration has been complete");
}

void Daemon09::reconfigure() {
    try {
        configure();
    }
    catch (std::exception const& exception) {
        syslog(LOG_ERR, "%s", exception.what());
    }
}

void Daemon09::work() {
    std::string const img = "/IMG ";
    std::string const others = "/OTHERS ";
    std::string const ignore = "2>/dev/null ";
    std::string const rm = "rm -rf ";
    std::string const mkdir = "mkdir -p ";
    std::string const mv = "mv -n ";
    std::string const png = "/*.png ";
    std::string const any = "/* ";
    int microsecond_multiplier = 1000000;

    while (!m_stop) {
        // clear directory
        std::string command = rm + m_dir_to + any;
        if (system(command.c_str()) != 0) {
            syslog(LOG_ALERT, "unable to clear directory");
            usleep(microsecond_multiplier * m_interval_s);
            continue;
        }

        // create subdirs
        command = mkdir + m_dir_to + img + ignore;
        if (system(command.c_str()) != 0) {
            syslog(LOG_ALERT, "unable to make directories");
            usleep(microsecond_multiplier * m_interval_s);
            continue;
        }
        command = mkdir + m_dir_to + others + ignore;
        if (system(command.c_str()) != 0) {
            syslog(LOG_ALERT, "unable to make directories");
            usleep(microsecond_multiplier * m_interval_s);
            continue;
        }

        // move files
        command = mv + m_dir_from + png + m_dir_to + img + ignore;
        if (system(command.c_str()) != 0) {
            syslog(LOG_DEBUG, "unable to move /*.png items");
        }
        command = mv + m_dir_from + any + m_dir_to + others + ignore;
        if (system(command.c_str()) != 0) {
            syslog(LOG_DEBUG, "unable to move /* items");
        }

        syslog(LOG_DEBUG, "daemon loop has been successfully executed");
        usleep(microsecond_multiplier * m_interval_s);
    }

}
