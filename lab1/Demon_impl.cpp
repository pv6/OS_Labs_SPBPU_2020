//
// Created by yudzhinnsk on 30.10.2020.
//
#include "NeededInfo.h"
#include "SubMethods.h"
#include "Demon.h"
NeededInfo* Demon::nf = nullptr;
Demon::Demon(NeededInfo &nInf){
    nf = new NeededInfo(nInf.retFolderPath(),nInf.retConfigPath(), nInf.retPId(),nInf.retIntervalValue());
}
void Demon::createDemon(){
    pid_t pid;

    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    if (setsid() < 0)
        exit(EXIT_FAILURE);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    pid = fork();

    if (pid < 0)
        exit(EXIT_FAILURE);

    if (pid > 0)
        exit(EXIT_SUCCESS);

    umask(0);

    if (chdir("/") < 0) {
        syslog (LOG_NOTICE, "Error occured while changing working directory.");
        exit(EXIT_FAILURE);
    }

    syslog (LOG_NOTICE, "New daemon.");

    kill_last_daemon();

    /* set new pid */

    std::ofstream pid_file_out(nf->retPId().c_str());

    if (pid_file_out.is_open()) {
        pid_file_out << getpid();
        pid_file_out.close();
    }
}
void Demon::kill_last_daemon() {
    std::ifstream pid_file(nf->retPId().c_str());

    if (pid_file.is_open() && !pid_file.eof()) {
        pid_t prev_daemon_pid;
        pid_file >> prev_daemon_pid;

        if (prev_daemon_pid > 0) {
            kill(prev_daemon_pid, SIGTERM);
        }
    }

    pid_file.close();
}
void Demon::signal_handler(int signum) {
    if (signum == SIGHUP) {
        syslog (LOG_NOTICE, "SIGHUP signal caught.");
        if (SubMethods::read_config_file(*nf) != EXIT_SUCCESS) {
            syslog(LOG_ERR, "Can't read config file");
            kill_last_daemon();
        }
    }
    else if (signum == SIGTERM) {
        syslog (LOG_NOTICE, "SIGTERM signal caught.");
        unlink(nf->retPId().c_str());
        exit(0);
    }
}
int Demon::deleteSubFolders(std::string &path, int deepLevel){
    if(path.empty()){
        std::cout << "bad path" << std::endl;
        return EXIT_FAILURE;
    }

    char folderRemove[255];
    strcpy(folderRemove, path.c_str());
    DIR *theFolder = opendir(folderRemove);
    struct dirent *next_file;
    struct stat st{};
    int rc = 0;
    char filepath[600];

    while((next_file = readdir(theFolder)) != NULL ){
        sprintf(filepath, "%s/%s", folderRemove, next_file->d_name);
        rc = stat(filepath, &st);
        if (rc != 0 ) {
            printf("Ошибка при вызове stat('%s')\n", filepath);
            exit(-1);
        }

        if(S_ISDIR(st.st_mode)){
            char test = next_file->d_name[0];
            if(test != '.'){
                deleteSubFolders(reinterpret_cast<std::string &>(filepath), ++deepLevel);
                deepLevel--;
            }
        }
        if(deepLevel != 0){
            remove(filepath);
        }
    }
    if(deepLevel != 0) {
        remove(folderRemove);
    }
    closedir(theFolder);
    return EXIT_SUCCESS;
}
void Demon::runDemon(){
    while (1) {
        std::string folderPath = nf->retFolderPath();
        if (!SubMethods::is_dir_exist(folderPath))
            printf("WARNING: Destination directory %s does not exist. It will be created. \n", folderPath.c_str());

        deleteSubFolders(nf->retFolderPath(), 0);
        sleep (nf->retIntervalValue());
    }

}

