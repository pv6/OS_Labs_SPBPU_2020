#include "Daemon.h"
#include "ConfigReader.h"
#include "FolderWorker.h"
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>

Daemon* Daemon::_instance = nullptr;
const char* pidFilePath = "/tmp/lab1.pid";

Daemon* Daemon::get() {
    if (_instance == nullptr)
        _instance = new Daemon();
    return _instance;
}

void Daemon::run(const char* configFilePath) {
    char* buff = realpath(configFilePath, NULL);
    if (buff != NULL) {
        _configFilePath = buff;
    	free(buff);
    }

    std::string folder1Path, folder2Path;
    size_t oldDefTime;
    try {
        ConfigReader reader(_configFilePath);
        folder1Path = reader.getFolder1Path();
        folder2Path = reader.getFolder2Path();
        oldDefTime = reader.getOldDefTime();

        _updateTime = reader.getUpdateTime();
    } catch (Error error) {
        _lastError = error;
        throw error;
    }

    _folderWorker = new FolderWorker(folder1Path, folder2Path, oldDefTime);
    if (_folderWorker == nullptr) {
        _lastError = Error::UNKNOWN;
        syslog(LOG_ERR, "Failed to allocate memory for _folderWorker for some reason");
        throw Error::UNKNOWN;
    }

    while(_active) {
        try { _folderWorker->work(); }
        catch (Error error) {
            _lastError = error;
            throw error;
        }
        sleep(_updateTime);
    }
}

Daemon::Daemon() {
    pidFileCheck();

    openlog("lab1_log", LOG_PID, LOG_DAEMON);

    pid_t pid;

    // Fork parent process
    pid = fork();
    if (pid < 0) {
    	syslog(LOG_ERR, "Failed to fork [1]");
        throw Error::FORK_ERROR;
    }

    if (pid > 0)
        throw Error::OK;

    // Create SID for child process
    if (setsid() < 0) {
    	syslog(LOG_ERR, "Failed to create SID");
        throw Error::FORK_ERROR;
    }

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, Daemon::handleSIGHUP);
    signal(SIGTERM, Daemon::handleSIGTERM);

    pid = fork();
    if (pid < 0) {
    	syslog(LOG_ERR, "Failed to fork [2]");
        throw Error::FORK_ERROR;
    }

    if (pid > 0)
        throw Error::OK;

    initPidFile();

    // Change the file mode mask
    umask(0);

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    syslog(LOG_NOTICE, "Daemon started");
}

Daemon::~Daemon() {
    Error lastError = _instance->_lastError;
    if (lastError != Error::OK)
        syslog(LOG_NOTICE, "Daemon terminated with an error, code %zu", (size_t)lastError);
    else
        syslog(LOG_NOTICE, "Daemon terminated");
    closelog();
    clearPidFile();
    delete _folderWorker;
}

void Daemon::pidFileCheck() {
    FILE* pidFile = fopen(pidFilePath, "r");
    if (pidFile == NULL)
    	return;
    pid_t activePid;
    fscanf(pidFile, "%d", &activePid);
    fclose(pidFile);

    if (activePid != 0) {
    	std::string msg = "kill -15 " + std::to_string(activePid);
    	system(msg.c_str());
    }
}

void Daemon::initPidFile() {
    FILE* pidFile = fopen(pidFilePath, "w+");
    fprintf(pidFile, "%d", getpid());
    fclose(pidFile);
}

void Daemon::clearPidFile() {
    FILE* pidFile = fopen(pidFilePath, "w+");
    fclose(pidFile);
}

void Daemon::handleSIGTERM(int signum) {
    _instance->_active = false;
}

void Daemon::handleSIGHUP(int signum) {
    if (_instance->_folderWorker == nullptr)
    	return;

    std::string folder1Path, folder2Path;
    size_t oldDefTime, updateTime;

    try {
    	ConfigReader reader(_instance->_configFilePath);
    	folder1Path = reader.getFolder1Path();
    	folder2Path = reader.getFolder2Path();
    	oldDefTime = reader.getOldDefTime();
    	updateTime = reader.getUpdateTime();
    } catch (Error error) {
    	syslog(LOG_WARNING, "Exception occured while reading configuration file, daemon will continue to work with previous configuration");
    	return;
    }

    syslog(LOG_NOTICE, "New configuration was uploaded from [%s]", _instance->_configFilePath.c_str());
    _instance->_updateTime = updateTime;
    _instance->_folderWorker->setConfiguration(folder1Path, folder2Path, oldDefTime);
}
