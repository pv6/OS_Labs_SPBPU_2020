#include "daemon.h"

/*bool is_dir_exist(std::string& path) {
    if (path[0] == '~') {
         passwd *pw;
         uid_t uid;

        uid = geteuid();
        pw = getpwuid(uid);
        if (pw)
            path.replace(0, 1, std::string("/home/") + pw->pw_name);
        else
            syslog(LOG_WARNING, "WARNING: Couldn't find username by UID %u. There is no guarantee to find folder which path contains `~`.", uid);

    }
    struct stat buffer;
    if (path.length() != 0 && stat(path.c_str(), &buffer) == 0 && (S_ISDIR(buffer.st_mode)))
        return true;
    return false;
}

bool is_file_exist(const std::string &name) {
    struct stat buffer;
    if (name.length() != 0 && stat(name.c_str(), &buffer) == 0)
        return true;
    return false;
}
    void remove_dir_content(const char* path, int level = 0)
    {
        struct dirent *de;
        char fname[300];
        DIR *dr = opendir(path);
        printf("%s\n", path);
        if(dr == NULL)
        {
            printf("No file or directory found\n");
            return;
        }
        while((de = readdir(dr)) != NULL)
        {
            int ret = -1;
            struct stat statbuf;
            sprintf(fname,"%s/%s",path,de->d_name);
            if (!strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."))
                        continue;
            if(!stat(fname, &statbuf))
            {
                if(S_ISDIR(statbuf.st_mode))
                {
                    printf("Is dir: %s\n",fname);
                    printf("Err: %d\n",ret = unlinkat(dirfd(dr),fname,AT_REMOVEDIR));
                    if(ret != 0)
                    {
                        remove_dir_content(fname, level + 1);
                        printf("Err: %d\n",ret = rmdir(fname));
                    }
                }
                else
                {
                    if (level > 0){
                       printf("Is file: %s\n",fname);
                       printf("Err: %d\n",unlink(fname));
                    }
                }
            }
        }
        closedir(dr);
    }
int read_config_file() {
    if (!is_file_exist(config_path.c_str())) {
        printf("Config file %s does not exist. \n", config_path.c_str());
        return EXIT_FAILURE;
    }
    std::ifstream config_file(config_path.c_str());
    if (config_file.is_open() && !config_file.eof()) {
        interval = 0;
        config_file >> folder1 >> interval;
        config_file.close();
        config_path = realpath(config_path.c_str(), NULL);
    }
    if (folder1.length() == 0 || interval == 0) {
        printf("Empty args, please check config file %s. \n", config_path.c_str());
        return EXIT_FAILURE;
    }

    if (!is_dir_exist(folder1)) {
        printf("Directory %s does not exist. \n", folder1.c_str());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void kill_last_daemon() {
    std::ifstream pid_file(pid_file_path.c_str());

    if (pid_file.is_open() && !pid_file.eof()) {
        pid_t prev_daemon_pid;
        pid_file >> prev_daemon_pid;

        if (prev_daemon_pid > 0) {
            kill(prev_daemon_pid, SIGTERM);
        }
    }

    pid_file.close();
}

void signal_handler(int signum) {
    if (signum == SIGHUP) {
        syslog (LOG_NOTICE, "SIGHUP signal caught.");
        if (read_config_file() != EXIT_SUCCESS) {
            syslog(LOG_ERR, "Can't read config file");
            kill_last_daemon();
        }
    }
    else if (signum == SIGTERM) {
        syslog (LOG_NOTICE, "SIGTERM signal caught.");
        unlink(pid_file_path.c_str());
        exit(0);
    }
}


static void create_daemon() {
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


    std::ofstream pid_file_out(pid_file_path.c_str());

    if (pid_file_out.is_open()) {
        pid_file_out << getpid();
        pid_file_out.close();
    }

}*/

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Error: exprected 2 arguments, got %d \n", argc);
        return EXIT_FAILURE;
    }

    openlog ("lab1_daemons", LOG_PID, LOG_DAEMON);
    syslog (LOG_NOTICE, "Open syslog.");

    file_handler* fh = file_handler::getInstance(std::string(argv[1]));
    if (fh == nullptr){
        syslog (LOG_ERR, "File handler isn't created.");
        return EXIT_FAILURE;
    }

    int last_error = fh->parse_config();
    if (last_error != EXIT_SUCCESS){
        syslog (LOG_ERR, "Config file isn't parsed.");
        return last_error;
    }
    
    fflush(stdout);

    daemon_parent* new_daemon = daemon_parent::getInstance(fh);
    if (new_daemon == nullptr){
        syslog (LOG_ERR, "Daemon instance isn't create.");
        return EXIT_FAILURE;
    }
    try{
        new_daemon->create();
        while (true) {
            new_daemon->run_process(fh->getFolder().c_str());
            sleep (fh->getInterval());
        }
    } catch(int e){
        if (e == EXIT_FAILURE)
            return EXIT_FAILURE;
        if (e == EXIT_SUCCESS)
            return EXIT_SUCCESS;
    }

    syslog (LOG_NOTICE, "Close syslog.");
    closelog();

    delete new_daemon;

    return EXIT_SUCCESS;
}
