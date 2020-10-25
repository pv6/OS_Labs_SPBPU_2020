#include "daemon.h"

std::string daemon::config_path;
std:: string daemon::pid_path;
std::string daemon::dir1;
std::string daemon::dir2;
size_t daemon::interval;

void daemon::daemon_fork() {
    pid_t pid = fork();
    if (pid == -1){
        syslog(LOG_ERR, "Daemon start failed");
        exit(EXIT_FAILURE);
    }
    else if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
}

int daemon::delete_prev_daemon() {
    std:: ifstream pid_file(pid_path);
    if (pid_file.is_open() && !pid_file.eof()) {
        pid_t prev;
        pid_file >> prev;
        if (prev > 0) {
            kill(prev, SIGTERM);
        }
        pid_file.close();
    }
    else{
        syslog(LOG_ERR, "Could nit open pid file %s", pid_path.c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int daemon::set_pid_file(){
    std:: ofstream pid_file(pid_path);
    if(pid_file.is_open()){
        pid_file << getpid();
        pid_file.close();
    }
    else{
        syslog(LOG_ERR, "Could nit open pid file %s", pid_path.c_str());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
int daemon::create() {
    read_config();
    daemon_fork();
    openlog("daemon_lab", LOG_NOWAIT | LOG_PID, LOG_DAEMON);
    umask(0);
    if (setsid() < 0)
    {
        syslog(LOG_ERR, "Could not generate session ID for child process");
        exit(EXIT_FAILURE);
    }
    daemon_fork();
    if ((chdir("/")) < 0)
    {
        syslog(LOG_ERR, "Could not change working directory to /");
        exit(EXIT_FAILURE);
    }
    syslog(LOG_NOTICE, "Successfully started daemon_lab");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    if (delete_prev_daemon() == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    if (set_pid_file() == EXIT_FAILURE){
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
void daemon:: signal_handler(int signal) {
    switch (signal) {
        case SIGHUP:
            syslog(LOG_NOTICE, "Reload new config");
            if (read_config() == EXIT_FAILURE) {
                exit(EXIT_FAILURE);
            }
            break;
        case SIGTERM:
            syslog(LOG_NOTICE, "Terminate deamon by signal");
            closelog();
            exit(0);
        default:
            syslog(LOG_NOTICE, "Unknown signal");
            break;
    }
}

void daemon::set_config(std::string &conf_file) {
    std:: string work_dir = std::string(get_current_dir_name());
    pid_path = work_dir +"/" + PID_FILE;
    config_path = work_dir + "/" + conf_file;
}

int daemon::read_config(){
    std:: ifstream conf_file(config_path);
    if(conf_file.is_open() && !conf_file.eof()) {
        conf_file >> dir1 >> dir2 >> interval;
        conf_file.close();
    }
    else{
        syslog(LOG_NOTICE, "Can not read config file\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void daemon::move(const char* src_path, const char* dst_path) {
    int source = open(src_path, O_RDONLY, 0);
    int dest = open(dst_path, O_WRONLY | O_CREAT , 0644);
    struct stat stat_source;
    fstat(source, &stat_source);
    sendfile(dest, source, nullptr, stat_source.st_size);
    close(source);
    close(dest);
    if (std::remove(src_path) != 0) {
        syslog(LOG_NOTICE, "Could not move file");
    }
}

void daemon::work(const std::string& dir1, const std::string& dir2, Mode mode) {
    struct dirent *entry_ptr;
    struct stat t_stat;
    time_t creation_time, cur_time;

    DIR *dir;
    try {
        dir = opendir(dir1.c_str());
    }
    catch (...) {
        syslog(LOG_ERR, "Problem with directory: %s", dir1.c_str());
        exit(EXIT_FAILURE);
    }

    time(&cur_time);
    while ((entry_ptr = readdir(dir)) != nullptr) {
        if (entry_ptr->d_type == DT_DIR) {
            continue;
        }

        std::string src_path = dir1 + std::string("/") + std::string(entry_ptr->d_name);
        std::string dst_path = dir2 + std::string("/") + std::string(entry_ptr->d_name);
        stat(src_path.c_str(), &t_stat);
        creation_time = t_stat.st_atime;
        switch (mode) {
            case MODE_0:
                if (cur_time - creation_time > interval) {
                    move(src_path.c_str(), dst_path.c_str());
                }
                break;
            case MODE_1:
                if (cur_time - creation_time < interval) {
                    move(src_path.c_str(), dst_path.c_str());
                }
                break;
        }
    }
    closedir(dir);
}

void daemon::run() {
    while (true)
    {
        work(dir1, dir2, MODE_0);
        work(dir2, dir1, MODE_1);
        sleep(interval);
    }
}