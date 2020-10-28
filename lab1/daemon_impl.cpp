#include "daemon.h"

void daemon_parent::create(){
    pid_t pid;

    pid = fork();

    if (pid < 0)
        throw EXIT_FAILURE;

    if (pid > 0)
        throw EXIT_SUCCESS;

    if (setsid() < 0)
        throw EXIT_FAILURE;

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);

    pid = fork();

    if (pid < 0)
        throw EXIT_FAILURE;

    if (pid > 0)
        throw EXIT_SUCCESS;

    umask(0);

    if (chdir("/") < 0) {
        syslog (LOG_NOTICE, "Error occured while changing working directory.");
        throw EXIT_FAILURE;
    }

    syslog (LOG_NOTICE, "New daemon.");

    kill_last_daemon();

    std::ofstream pid_file_out(fh->getPIDFilePath().c_str());

    if (pid_file_out.is_open()) {
        pid_file_out << getpid();
        pid_file_out.close();
    }
}

void daemon_parent::signal_handler(int signal_id){
    if (signal_id == SIGHUP) {
        syslog (LOG_NOTICE, "SIGHUP signal caught.");
        if (fh->parse_config() != EXIT_SUCCESS) {
            syslog (LOG_ERR, "Can't read config file.");
            kill_last_daemon();
        }
    }
    else if (signal_id == SIGTERM) {
        syslog (LOG_NOTICE, "SIGTERM signal caught.");
        unlink(fh->getPIDFilePath().c_str());
        throw EXIT_SUCCESS;
    }
}

void daemon_parent::kill_last_daemon(){
    std::ifstream pid_file(fh->getPIDFilePath().c_str());

    if (pid_file.is_open() && !pid_file.eof()) {
        pid_t prev_daemon_pid;
        pid_file >> prev_daemon_pid;

        if (prev_daemon_pid > 0) {
            kill(prev_daemon_pid, SIGTERM);
        }
    }
    pid_file.close();
}

void cur_daemon::run_process(const char* path, int level = 0){
    struct dirent *de;
    char fname[300];
    DIR *dr = opendir(path);
    syslog (LOG_NOTICE, "Process in directory: %s.", path);
    if(dr == NULL){
        syslog (LOG_NOTICE, "No directory found.");
        return;
    }
    while((de = readdir(dr)) != NULL)
    {
        int ret = -1;
        struct stat statbuf;
        sprintf(fname, "%s/%s", path, de->d_name);
        if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
            continue;
        if(!stat(fname, &statbuf)){
            if(S_ISDIR(statbuf.st_mode)){
                syslog (LOG_NOTICE, "Is dir: %s.", fname);
                syslog (LOG_NOTICE, "Err_code: %d.", ret = unlinkat(dirfd(dr),fname,AT_REMOVEDIR));
                if(ret != 0){
                    run_process(fname, level + 1);
                    syslog (LOG_NOTICE, "Err_code: %d.", ret = rmdir(fname));
                }
            }
            else{
                if (level > 0){
                    syslog (LOG_NOTICE, "Is file: %s.",fname);
                    syslog (LOG_NOTICE, "Err_code: %d.",unlink(fname));
                }
            }
        }
    }
    closedir(dr);
}

daemon_parent* daemon_parent::getInstance(file_handler* fh_arg){
   if (!fh_arg){
      syslog(LOG_WARNING, "WARNING: File handler is not created.");
      return nullptr;
   }
   daemon_parent::fh = fh_arg;
   return cur_daemon::getInstance();
}

file_handler* daemon_parent::fh = nullptr;
cur_daemon* cur_daemon::cd_instance = nullptr;
