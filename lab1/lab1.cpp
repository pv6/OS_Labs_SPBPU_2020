#include <stdio.h>
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <pwd.h>
#include <string.h>
#include <dirent.h>
#include <iostream>

class NeededInfo{
public:
    NeededInfo(std::string path){
        this->configPath = path;
    }
    NeededInfo(std::string fPath, std::string cPath, std::string Pid, unsigned int val){
        this->configPath = cPath;
        this->FolderPath = fPath;
        this->PId = Pid;
        this->IntervalValue = val;
    }
    static NeededInfo* create(std::string fPath, std::string cPath, std::string Pid, unsigned int val){
        return new NeededInfo(fPath,cPath,Pid,val);
    }
    void setFolderPath(std::string path){
        this->FolderPath = path;
    }
    void setIntervalVal(unsigned int val){
        this->IntervalValue = val;
    }
    void setPId(std::string val){
        this->PId = val;
    }
    std::string retFolderPath(){
        return FolderPath;
    }
    unsigned int retIntervalValue(){
        return IntervalValue;
    }
    std::string retConfigPath(){
        return configPath;
    }
    std::string retPId(){
        return PId;
    }
private:
    std::string FolderPath;
    std::string configPath;
    std::string  PId = "/var/run/lab1";
    unsigned int IntervalValue = 0;
};

static bool is_dir_exist(std::string& path) {
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

static bool is_file_exist(const std::string &name) {
    struct stat buffer;
    if (name.length() != 0 && stat(name.c_str(), &buffer) == 0)
        return true;
    return false;
}

static int read_config_file(NeededInfo &nf) {
    std::string config_path = nf.retConfigPath();
    if (!is_file_exist(config_path.c_str())) {
        printf("Config file %s does not exist. \n", config_path.c_str());
        return EXIT_FAILURE;
    }
    std::string file = "";
    unsigned int val = 0;
    std::ifstream config_file(config_path.c_str());
    if (config_file.is_open() && !config_file.eof()) {
        config_file >> file >> val;
        config_file.close();
        config_path = realpath(config_path.c_str(), NULL);
    }
    if (file.length() == 0 || val == 0) {
        printf("Empty args, please check config file %s. \n", config_path.c_str());
        return EXIT_FAILURE;
    }

    nf.setFolderPath(file);
    nf.setIntervalVal(val);

    if (!is_dir_exist(file)) {
        printf("Directory %s does not exist. \n", nf.retFolderPath().c_str());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
} //TODO: OKEY

class Demon{
public:
    static NeededInfo *nf;
    Demon(NeededInfo &nInf){
        nf = NeededInfo::create(nInf.retFolderPath(),nInf.retConfigPath(), nInf.retPId(),nInf.retIntervalValue());
    }
    void createDemon(){
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
    static void kill_last_daemon() {
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
    static void signal_handler(int signum) {
        if (signum == SIGHUP) {
            syslog (LOG_NOTICE, "SIGHUP signal caught.");
            if (read_config_file(*nf) != EXIT_SUCCESS) {
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
};

NeededInfo* Demon::nf = nullptr;

int deleteSubFolders(std::string path, int deepLevel){
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
    char filepath[605];

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
                deleteSubFolders(filepath, ++deepLevel);
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

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Error: expected 2 arguments, got %d \n", argc);
        return EXIT_FAILURE;
    }

    openlog ("lab1", LOG_PID, LOG_DAEMON);
    syslog (LOG_NOTICE, "Open syslog");

    NeededInfo nf = NeededInfo(argv[1]);
    int readConfigFile = read_config_file(nf);
    if (readConfigFile != EXIT_SUCCESS)
        return readConfigFile;

    fflush(stdout);
    Demon dm = Demon(nf);
    dm.createDemon();

    while (1) {
        std::string folderPath = nf.retFolderPath();
        if (!is_dir_exist(folderPath))
            printf("WARNING: Destination directory %s does not exist. It will be created. \n", folderPath.c_str());

        deleteSubFolders(nf.retFolderPath(), 0);
        sleep (nf.retIntervalValue());
    }

    syslog (LOG_NOTICE, "Close syslog");
    closelog();

    return EXIT_SUCCESS;
}
