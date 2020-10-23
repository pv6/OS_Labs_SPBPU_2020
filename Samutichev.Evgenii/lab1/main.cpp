#include "ConfigReader.h"
#include "Error.h"
#include "FolderWorker.h"
#include <iostream>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <syslog.h>

const char* pidFilePath = "/tmp/lab1.pid";

FolderWorker* g_folderWorker = nullptr;
bool g_stopped = false;
std::string g_configFilePath;
size_t g_updateTime;

void handleSIGTERM(int signum);

void handleSIGHUP(int signum);

pid_t pidFileCheck();

void clearPidFile();

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "You should specify configuration file\n";
        return EXIT_FAILURE;
    }
    
    pid_t activePid = pidFileCheck();
    if (activePid != 0) {
    	std::string msg = "kill -15 " + std::to_string(activePid);
    	system(msg.c_str());
    }

    openlog("lab1_log", LOG_PID, LOG_DAEMON);

    std::string folder1Path, folder2Path;
    size_t oldDefTime;
    g_configFilePath = argv[1];
    char* buff = realpath(g_configFilePath.c_str(), NULL);
    
    if (buff != NULL) {
    	g_configFilePath = buff;
    	free(buff);
    }
    
    try {
        ConfigReader reader(g_configFilePath);
        folder1Path = reader.getFolder1Path();
        folder2Path = reader.getFolder2Path();
        oldDefTime = reader.getOldDefTime();
        
        g_updateTime = reader.getUpdateTime();
    }
    catch (Error error) {
        std::cout << "An unhandling exception occured while reading configuration file, error code " << (size_t)error << ", check syslog for details\n";
        closelog();
        return EXIT_FAILURE;
    }

    pid_t pid;

    // Fork parent process
    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    // Create SID for child process
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, handleSIGHUP);
    signal(SIGTERM, handleSIGTERM);

    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0) 
        exit(EXIT_SUCCESS);
    
    // Writing to the pid file
    FILE* pidFile = fopen(pidFilePath, "w+");
    fprintf(pidFile, "%d", getpid());
    fclose(pidFile);
    
    // Change the file mode mask
    umask(0);

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    syslog(LOG_NOTICE, "Daemon started");

    g_folderWorker = new FolderWorker(folder1Path, folder2Path, oldDefTime);
    if (g_folderWorker == nullptr) {
    	syslog(LOG_ERR, "Failed to allocate memory for g_folderWorker for some reason");
    	closelog();
    	return EXIT_FAILURE;
    }

    while(1) {
    	if (g_stopped)
    	    break;
    	    
        try {
            g_folderWorker->work();
        } catch (Error error) {
            syslog(LOG_NOTICE, "Daemon terminated with an error, code %zu", (size_t)error);
            closelog();
            clearPidFile();
            
            delete g_folderWorker;
            
            return EXIT_FAILURE;
   	}
        
        sleep(g_updateTime);
    }

    syslog(LOG_NOTICE, "Daemon terminated");
    closelog();
    delete g_folderWorker;
    clearPidFile();
    return EXIT_SUCCESS;
}

void handleSIGTERM(int signum) {
    g_stopped = true;
}

void handleSIGHUP(int signum) {
    if (g_folderWorker == nullptr)
    	return;
    	
    std::string folder1Path, folder2Path;
    size_t oldDefTime, updateTime;
    
    try {
    	ConfigReader reader(g_configFilePath);
    	folder1Path = reader.getFolder1Path();
    	folder2Path = reader.getFolder2Path();
    	oldDefTime = reader.getOldDefTime();
    	updateTime = reader.getUpdateTime();
    } catch (Error error) {
    	syslog(LOG_WARNING, "Exception occured while reading configuration file, daemon will work with previous configuration");
    	return;
    }
    
    syslog(LOG_NOTICE, "New configuration was uploaded from [%s]", g_configFilePath.c_str());
    g_updateTime = updateTime;
    g_folderWorker->setConfiguration(folder1Path, folder2Path, oldDefTime);
}

pid_t pidFileCheck() {
    FILE* pidFile = fopen(pidFilePath, "r");
    if (pidFile == NULL)
    	return 0;
    pid_t res;
    fscanf(pidFile, "%d", &res);
    fclose(pidFile);
    return res;
}

void clearPidFile() {
    FILE* pidFile = fopen(pidFilePath, "w+");
    fclose(pidFile);
}
