#include "daemon.h"

std::string daemon::config_path;
std:: string daemon::pid_path;
std::string daemon::dir1;
std::string daemon::dir2;
size_t daemon::interval;

bool daemon::daemon_fork() {
    pid_t pid = fork();
    if (pid == -1){
        throw std::runtime_error("Daemon fork was failed");
    }
    else if (pid > 0) {
        return false;
    }
    return true;
}

void daemon::delete_prev_daemon() {
    std:: ifstream pid_file(pid_path);
    if (pid_file.is_open()) {
        pid_t prev;
        pid_file >> prev;
        if (prev > 0) {
            kill(prev, SIGTERM);
        }
        pid_file.close();
        syslog(LOG_NOTICE, "Previous daemon was killed");
    }
    else{
        std::string err = "Could not open pid file: " + pid_path;
        throw std::runtime_error(err);
    }
}

void daemon::set_pid_file(){
    std:: ofstream pid_file(pid_path);
    if(pid_file.is_open()){
        pid_file << getpid();
        pid_file.close();
        syslog(LOG_NOTICE, "PID was saved");
    }
    else{
        std::string err = "Could not open pid file: " + pid_path;
        throw std::runtime_error(err);
    }
}
bool daemon::create() {
    read_config();

    if (!daemon_fork()){
        return false;
    }

    umask(0);
    if (setsid() < 0)
        throw std::runtime_error("Could not generate session ID for child process");

    if (!daemon_fork()){
        return false;
    }

    if ((chdir("/")) < 0)
        throw std::runtime_error("Could not change working directory to /");

    syslog(LOG_NOTICE, "Successfully started child process");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    signal(SIGHUP, signal_handler);
    signal(SIGTERM, signal_handler);
    syslog(LOG_NOTICE, "Signal handler was updated");

    delete_prev_daemon();
    set_pid_file();

    syslog(LOG_NOTICE, "Daemon was set up");
    return true;
}
void daemon:: signal_handler(int signal) {
    switch (signal) {
        case SIGHUP:
            syslog(LOG_NOTICE, "Config was reload");
            read_config();
            break;
        case SIGTERM:
            syslog(LOG_NOTICE, "Terminate daemon by signal");
            closelog();
            finish();
        default:
            syslog(LOG_NOTICE, "Unknown signal");
            break;
    }
}

void daemon::set_config(std::string &conf_file) {
    std:: string work_dir = std::string(get_current_dir_name());
    pid_path = PID_FILE;
    config_path = work_dir + "/" + conf_file;
}

void daemon::read_config(){
    std:: ifstream conf_file(config_path);
    if(conf_file.is_open() && !conf_file.eof()) {
        conf_file >> dir1 >> dir2 >> interval;
        conf_file.close();
    }
    else{
        throw std::runtime_error("Could not read config file");
    }
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
        throw std::runtime_error("Could not remove file");
    }
}

void daemon::work(const std::string& dir1, const std::string& dir2, Mode mode) {
    struct dirent *entry_ptr;
    struct stat t_stat;
    time_t creation_time, cur_time;

    DIR *dir;
    dir = opendir(dir1.c_str());
    if (!dir){
        std::string err = "Could not open directory: " + dir1;
        throw std::runtime_error(err);
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
    is_finished = false;
    syslog(LOG_NOTICE, "Successfully run daemon_lab process");
    int i = 0;
    while (!is_finished)
    {
        syslog(LOG_NOTICE, "Successfully done %i operation", ++i);
        work(dir1, dir2, MODE_0);
        work(dir2, dir1, MODE_1);
        sleep(interval);
    }
}

void daemon::finish() {
    is_finished = true;
    syslog(LOG_NOTICE, "Daemon process was finished");
}
