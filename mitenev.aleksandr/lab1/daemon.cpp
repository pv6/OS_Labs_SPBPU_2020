//
// Created by aleksandr on 27.10.2020.
//
#include "daemon.h"
#include <syslog.h>
#include <csignal>
#include <sys/stat.h>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <cstring>
#include <map>
#include "parser.h"

const std::string Daemon::title_ = "Lab1 - daemon";
unsigned int Daemon::period_;
std::string Daemon::pidFile_ = "/var/run/lab1.pid";
std::string Daemon::dir1_;
std::string Daemon::dir2_;
std::string Daemon::homeDir_;
std::string Daemon::configFile_;
std::string Daemon::totalLog_;
bool Daemon::status_;

bool Daemon::init(const std::string &configFile) {
    openlog(title_.c_str(), LOG_PID | LOG_NDELAY, LOG_USER);
    syslog(LOG_INFO, "Run init");

    status_ = true;

    char buff[FILENAME_MAX];

    if(!getcwd(buff, FILENAME_MAX)){
        syslog(LOG_ERR, "Failed in getcwd: %d", errno);
        terminate();
        return false;
    }

    homeDir_ = buff;


    pid_t pid = fork();

    if (pid == -1) {
        syslog(LOG_ERR, "First fork failure");
        terminate();
        return false;
    } else if (pid != 0) {
        syslog(LOG_NOTICE, "Parent process");
        return false;
    }

    syslog(LOG_INFO, "First fork success");


    if (setsid() == -1) {
        syslog(LOG_ERR, "Failed in setsid: %d", errno);
        terminate();
        return false;
    }

    pid = fork();
    if (pid == -1) {
        syslog(LOG_ERR, "Second fork failure");
        terminate();
        return false;
    } else if (pid != 0) {
        syslog(LOG_NOTICE, "Parent process");
        return false;
    }

    syslog(LOG_INFO, "Second fork success");

    umask(0);

    if ((chdir("/")) == -1) {
        syslog(LOG_ERR, "Failed in chdir: %d", errno);
        terminate();
        return false;
    }

    if (close(STDIN_FILENO) == -1 ||
        close(STDOUT_FILENO) == -1 ||
        close(STDERR_FILENO) == -1) {
        syslog(LOG_ERR, "Failed in close: %d", errno);
        terminate();
        return false;
    }

    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);

    if(!handlePidFile()){
        syslog(LOG_ERR, "Failed in handlePidFile");
        terminate();
        return false;
    }

    if(!setConfig(configFile)){
        syslog(LOG_ERR, "Failed in setConfig");
        terminate();
        return false;
    }

    syslog(LOG_INFO, "Daemon is successfully initialized");

    return true;
}

bool Daemon::setPidFile() {
    std::ofstream pidStream(pidFile_);
    if (!pidStream.is_open()) {
        syslog(LOG_ERR, "Couldn't open pid file");
        return false;
    }
    pidStream << getpid();
    pidStream.close();
    return true;
}

bool Daemon::handlePidFile() {
    syslog(LOG_INFO, "Run handle PID file");
    std::ifstream pidFile(pidFile_);

    if (!pidFile.is_open()) {
        syslog(LOG_INFO, "Couldn't find/open old pid file");

    } else if (!pidFile.eof()) {
        pid_t oldPid;
        pidFile >> oldPid;
        pidFile.close();

        struct stat sb;
        std::string oldPath = "/proc/" + std::to_string(oldPid);

        if (stat(oldPath.c_str(), &sb) == 0)
            kill(oldPid, SIGTERM);
    }

    return setPidFile();
}

bool Daemon::setConfig(const std::string &configFile) {
    syslog(LOG_INFO, "Run set config");

    configFile_ = getAbsPath(configFile);
    if(configFile_.empty()) {
        syslog(LOG_ERR, "Config file not found");
        return false;
    }

    if(!readConfig()){
        return false;
    }

    syslog(LOG_INFO, "Set config completed successfully");
    return true;
}

void Daemon::signalHandler(int signal) {
    switch (signal) {
        case SIGHUP:
            syslog(LOG_INFO, "Updating config file");
            if(!readConfig()){
                terminate();
            }
            break;
        case SIGTERM:
            terminate();
            break;
        default:
            syslog(LOG_INFO, "Signal %i is not handled", signal);
    }
}

bool Daemon::readConfig() {
    syslog(LOG_INFO, "Run reading config");

    std::ifstream configFile(configFile_);
    if (!configFile.is_open()){
        syslog(LOG_ERR, "Couldn't open config file");
        return false;
    }

    std::map<Parser::ConfigParam, std::string> config;

    try {
        config = Parser::parseFile(configFile);
    } catch (std::runtime_error &runtime_error) {
        syslog(LOG_ERR, "%s", runtime_error.what());
        configFile.close();
        return false;
    }

    configFile.close();

    if (config.size() != 3){
        syslog(LOG_ERR, "There are not enough parameters in the config");
        return false;
    }
    if (config.find(Parser::PERIOD) != config.end()){
        try{
            long value = std::stol(config.at(Parser::PERIOD));
            if (value < 0) {
                syslog(LOG_ERR, "Invalid period");
                return false;
            }
            period_ = static_cast<uint>(value);
        }
        catch (std::exception &e) {
            syslog(LOG_ERR, "CONFIG ERROR: %s", e.what());
            return false;
        }
    }
    if (config.find(Parser::DIR1) != config.end()){
        dir1_ = config.at(Parser::DIR1);
        dir1_ = getAbsPath(dir1_);
    }
    if (config.find(Parser::DIR2) != config.end()){
        dir2_ = config.at(Parser::DIR2);
        dir2_ = getAbsPath(dir2_);
    }

    if(dir1_.empty() || dir2_.empty()){
        syslog(LOG_ERR, "Config parsing error: wrong directory name");
        return false;
    }
    totalLog_ = dir2_ + "/total.log";

    syslog(LOG_INFO, "period: %d, dir1: %s, dir2: %s, totalLog: %s",
           period_, dir1_.c_str(), dir2_.c_str(), totalLog_.c_str());

    return true;
}

std::string Daemon::getAbsPath(const std::string &path) {
    if (path.empty() || path[0] == '/')
        return path;


    std::string output = homeDir_ + "/" + path;

    char buf[_POSIX_PATH_MAX];
    char *res = realpath(output.c_str(), buf);
    if (!res) {
        syslog(LOG_ERR, "Couldn't find path: %s", path.c_str());
        return std::string();
    }

    output.assign(buf);

    return output;
}

void Daemon::terminate() {
    status_ = false;
    syslog(LOG_INFO, "Close log");
    closelog();
}

void Daemon::run() {
    syslog(LOG_INFO, "Run process");

    while (status_ && doWork()){
        sleep(period_);
    }
}

bool Daemon::doWork() {
    std::ofstream totalLog;
    totalLog.open(totalLog_, std::ostream::out | std::ostream::app);

    if (totalLog.is_open()){
        syslog(LOG_INFO, "Search log files");

        std::vector<std::string> logFiles = getAllLogs(dir1_);

        syslog(LOG_INFO, "%i files found", (int)logFiles.size());

        for (const std::string &logFile : logFiles)
        {
            writeLog(logFile, totalLog);
            if(unlink(logFile.c_str()) == -1){
                syslog(LOG_NOTICE, "%s wasn't delete: %d", logFile.c_str(), errno);
            }
        }

        totalLog.close();
    }
    else {
        syslog(LOG_ERR, "Couldn't open '%s'", totalLog_.c_str());
        return false;
    }
    return true;
}

std::vector<std::string> Daemon::getAllLogs(const std::string &path) {

    DIR *dir = opendir(path.c_str());

    if (!dir){
        syslog(LOG_ERR, "Couldn't open '%s'", path.c_str());
        return std::vector<std::string>();
    }

    std::vector<std::string> result;

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            std::string childDir = path + "/" + entry->d_name;
            std::vector<std::string> childLogs = getAllLogs(childDir);
            result.insert(result.end(), childLogs.begin(), childLogs.end());
        } else {
            std::string file = entry->d_name;

            std::string format = file.substr(file.find_last_of('.') + 1);

            if (format == "log") {
                file = path + "/" + file;
                result.push_back(file);
            }
        }
    }
    closedir(dir);

    return result;
}

void Daemon::writeLog(const std::string &log, std::ofstream &totalLog) {
    syslog(LOG_INFO, "Write : %s", log.c_str());

    totalLog << "\n\n" << log << "\n\n";

    std::ifstream file(log);
    if (!file.is_open())
    {
        syslog(LOG_ERR, "Couldn't open log file '%s'", log.c_str());
        return;
    }

    std::string content;
    file.seekg(0, std::ios::end);
    content.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(&content[0], content.size());
    file.close();

    totalLog << content;
}
