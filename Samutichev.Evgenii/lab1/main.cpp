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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "You should specify configuration file\n";
        return EXIT_FAILURE;
    }

    openlog("lab1_log", LOG_PID, LOG_DAEMON);

    std::string folder1Path, folder2Path;
    size_t updateTime, oldDefTime;
    try {
        ConfigReader reader(argv[1]);
        folder1Path = reader.getFolder1Path();
        folder2Path = reader.getFolder2Path();
        updateTime = reader.getUpdateTime();
        oldDefTime = reader.getOldDefTime();
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
    signal(SIGHUP, SIG_IGN);
    signal(SIGTERM, SIG_IGN);

    pid = fork();
    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    // Change the file mode mask
    umask(0);

    // Close standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    syslog(LOG_NOTICE, "Daemon started");

    FolderWorker folderWorker(folder1Path, folder2Path, oldDefTime);

    while(1) {
        try {
            folderWorker.work();
        }
        catch (Error error) {
            syslog(LOG_NOTICE, "Daemon terminated");
            closelog();
            exit(EXIT_FAILURE);
        }
        sleep(updateTime);
    }

    syslog(LOG_NOTICE, "Daemon terminated");
    closelog();
    return EXIT_SUCCESS;
}
