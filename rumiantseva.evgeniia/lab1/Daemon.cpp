//
// Created by Evgenia on 14.10.2020.
//

#include "Daemon.h"

const std::string Daemon::pid_path("/var/run/daemon.pid");

void Daemon::SignalHandler(int signal) {
    switch (signal) {
        case SIGHUP:
            syslog(LOG_NOTICE, "Updating configurations...");
            Daemon::GetDaemonInst().ReadConfig();
            break;
        case SIGTERM:
            Daemon::GetDaemonInst().Terminate();
            // removing pid-file
            unlink(Daemon::pid_path.c_str());
            syslog(LOG_NOTICE, "Daemon is terminated");
        default:
            syslog(LOG_NOTICE, "Signal %i is not handled", signal);
    }
}


void Daemon::ReadConfig() {
    syslog(LOG_NOTICE, "Reading config");
    is_running = false;
    std::ifstream configStream(config_path);
    if (!configStream.is_open()) {
        syslog(LOG_ERR, "Config file %s was not opened", config_path.c_str());
        std::string errDescription = "Config read error";
        throw CustomException(ERROR_EXIT, errDescription);
    }
    path_from.clear();
    path_to.clear();
    copy_interval = 0;
    configStream >> path_from >> path_to >> copy_interval >> new_file_time_limit;

    if (!path_from.length() || !path_to.length() || copy_interval <= 0 || new_file_time_limit <= 0) {
        syslog(LOG_ERR, "Invalid config format");
        std::string errDescription = "Config format error";
        throw CustomException(ERROR_EXIT, errDescription);
    }
    configStream.close();
    is_running = true;
    syslog(LOG_NOTICE, "Config is parsed");
}

Daemon::Daemon() {
    pid_t pid = fork();
    if (pid == -1) {
        syslog(LOG_ERR, "1st fork failure");
        std::string errDescription = "Fork error";
        throw CustomException(ERROR_EXIT, errDescription);
    } else if (pid != 0) {
        syslog(LOG_NOTICE, "Parent process");
        std::string errDescription = "After fork exit";
        throw CustomException(NORMAL_EXIT, errDescription);
    }
    if (setsid() < 0) {
        syslog(LOG_ERR, "Failed to create a session");
        std::string errDescription = "Session error";
        throw CustomException(ERROR_EXIT, errDescription);
    }
    pid = fork();
    if (pid == -1) {
        syslog(LOG_ERR, "2nd fork failure");
        std::string errDescription = "Fork error";
        throw CustomException(ERROR_EXIT, errDescription);
    } else if (pid != 0) {
        syslog(LOG_NOTICE, "Parent process");
        std::string errDescription = "After fork exit";
        throw CustomException(NORMAL_EXIT, errDescription);
    }
    // Сброс маски режима создания пользовательских файлов
    umask(0);

    if ((chdir("/")) < 0) {
        syslog(LOG_ERR, "Failed to change directory");
        std::string errDescription = "Directory error";
        throw CustomException(ERROR_EXIT, errDescription);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    CheckPrevPid();
    SavePid();
    syslog(LOG_NOTICE, "Daemon initiated");
}

void Daemon::CheckPrevPid() {
    std::ifstream pidStream(pid_path);
    if (!pidStream.is_open()) {
        syslog(LOG_ERR, "Smth wrong with PID file (was not opened to read)");
        std::string errDescription = "PID-file error";
        throw CustomException(ERROR_EXIT, errDescription);
    }
    if (!pidStream.eof()) {
        pid_t prevDaemonPid;
        std::string prevPath = "/proc/";
        pidStream >> prevDaemonPid;
        prevPath += std::to_string(prevDaemonPid);
        if (prevDaemonPid > 0 && FolderExists(prevPath)) {
            kill(prevDaemonPid, SIGTERM);
        }
    }
    pidStream.close();
}

void Daemon::SavePid() {
    std::ofstream pidStream(pid_path);
    if (!pidStream.is_open()) {
        syslog(LOG_ERR, "Smth wrong with PID file (was not opened to write)");
        std::string errDescription = "PID-file error";
        throw CustomException(ERROR_EXIT, errDescription);
    }
    pidStream << getpid();
    pidStream.close();
}

void Daemon::SetConfig(const std::string& configPath)
{
    char buf[_POSIX_PATH_MAX];
    char *res = realpath(configPath.c_str(), buf);
    if (!res) {
        syslog(LOG_ERR, "Could not find config");
        std::string errDescription = "Config file error";
        throw CustomException(ERROR_EXIT, errDescription);
    }
    config_path = std::string(buf);
    signal(SIGHUP, SignalHandler);
    signal(SIGTERM, SignalHandler);
    ReadConfig();
}

void Daemon::Run() {
    syslog(LOG_NOTICE, "Running...");
    is_running = true;
    while (!is_terminated) {
        if (is_running)
            CopyFiles();
        sleep(copy_interval);
    }
}

void Daemon::Terminate(){
    is_terminated = true;
}

void Daemon::CopyFiles() {
    syslog(LOG_NOTICE, "Copy process initiated...");
    DIR *folderFrom = OpenFolder(path_from);
    struct dirent *copiedFile;
    struct stat fileStat;
    std::string fileName;
    std::string filePath;
    time_t curTime;

    std::string newFilesDir = path_to + std::string("/NEW");
    CreateDir(newFilesDir);
    std::string oldFilesDir = path_to + std::string("/OLD");
    CreateDir(oldFilesDir);

    time(&curTime);

    while ((copiedFile = readdir(folderFrom)) != nullptr)
    {
        if (copiedFile->d_type != DT_DIR) {
            fileName = copiedFile->d_name;
            filePath = path_from + "/" + fileName;

            if (stat(filePath.c_str(), &fileStat) != 0)
                continue;

            if ((curTime - fileStat.st_atime) < new_file_time_limit) {
                std::string filePathUpdated = newFilesDir;
                filePathUpdated += "/" + fileName;
                CopyFile(filePath, filePathUpdated);
            } else {
                std::string filePathUpdated = oldFilesDir;
                filePathUpdated += "/" + fileName;
                CopyFile(filePath, filePathUpdated);
            }
        }

    }
    syslog(LOG_NOTICE, "Files were successfully copied!");
    closedir(folderFrom);
}

Daemon& Daemon::GetDaemonInst() {
    static Daemon daemonInst;
    return daemonInst;
}
