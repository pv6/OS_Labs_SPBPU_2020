//
// Created by Evgenia on 14.10.2020.
//

#include "Daemon.h"

const std::string Daemon::pid_path("/var/run/daemon.pid");

void Daemon::ReadConfig() {
    syslog(LOG_NOTICE, "Reading config");
    is_running = false;
    std::ifstream configStream(config_path);
    if (!configStream.is_open()) {
        syslog(LOG_ERR, "Config file %s was not opened", config_path.c_str());
        exit(EXIT_FAILURE);
    }
    path_from.clear();
    path_to.clear();
    copy_interval = 0;
    configStream >> path_from >> path_to >> copy_interval >> new_file_time_limit;

    if (!path_from.length() || !path_to.length() || copy_interval <= 0 || new_file_time_limit <= 0) {
        syslog(LOG_ERR, "Invalid config format");
        exit(EXIT_FAILURE);
    }
    configStream.close();
    is_running = true;
    syslog(LOG_NOTICE, "Config is parsed");
}

Daemon::Daemon() {
    pid_t pid = fork();
    if (pid == -1) {
        syslog(LOG_ERR, "1st fork failure");
        exit(EXIT_FAILURE);
    } else if (pid != 0) {
        syslog(LOG_NOTICE, "Parent process");
        exit(EXIT_SUCCESS);
    }
    if (setsid() < 0) {
        syslog(LOG_ERR, "Failed to create a session");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid == -1) {
        syslog(LOG_ERR, "2nd fork failure");
        exit(EXIT_FAILURE);
    } else if (pid != 0) {
        syslog(LOG_NOTICE, "Parent process");
        exit(EXIT_SUCCESS);
    }
    // Сброс маски режима создания пользовательских файлов
    umask(0);

    if ((chdir("/")) < 0) {
        syslog(LOG_ERR, "Failed to change directory");
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }
    pidStream << getpid();
    pidStream.close();
}

void Daemon::SetConfig(char *const configPath)
{
    config_path = configPath;
    ReadConfig();
}

void Daemon::Run() {
    syslog(LOG_NOTICE, "Running...");
    is_running = true;
    while (true) {
        if (is_running)
            CopyFiles();
        sleep(copy_interval);
    }
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

void Daemon::SetHandler(void (*signalHandler)(int)) {
    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);;
}
