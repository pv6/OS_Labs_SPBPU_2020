#include <cstdlib>
#include <cstdio>
#include <syslog.h>
#include <zconf.h>
#include <csignal>
#include <sys/stat.h>
#include <ftw.h>
#include "Daemon.h"
#include "parser.h"


std::string Daemon::pid = "/var/run/lab1.pid";
std::string Daemon::input = "dir1/";
std::string Daemon::output = "dir2/";
std::string Daemon::extension = "bk";
std::string Daemon::config;
std::string Daemon::homedir;
unsigned Daemon::intval = 30;
bool Daemon::exit = false;

void Daemon::run() {
    syslog(LOG_INFO, "Start work");
    exit = false;
    while (!exit) {
        cleanDst();
        copySrcToDst();
        sleep(intval);
    }
}

bool Daemon::init(std::string const &configFileName) {
    syslog(LOG_INFO, "Start init daemon");
    char buff[FILENAME_MAX];

    if (!getcwd(buff, FILENAME_MAX)) {
        throw std::runtime_error("Failed in getcwd");
    }
    homedir = buff;
    config = homedir + "/" + configFileName;
    if (!initTread()) {
        return false;
    }
    initSignals();
    readConfig();
    return true;
}

void Daemon::initSignals() {
    syslog(LOG_INFO, "Start init signals");
    signal(SIGHUP, signalHandler);
    signal(SIGTERM, signalHandler);
}

void Daemon::signalHandler(int signal) {
    switch (signal) {
        case SIGHUP:
            syslog(LOG_INFO, "Updating config file");
            readConfig();
            break;
        case SIGTERM:
            stop();
            break;
        default:
            syslog(LOG_INFO, "Signal %i is not handled", signal);
    }
}

void Daemon::stop() {
    syslog(LOG_INFO, "Stop working");
    unlink(pid.c_str());
    exit = true;
}

void Daemon::readConfig() {
    syslog(LOG_INFO, "Start read config");
    std::map<Parser::Param, std::string> parseResult;
    parseResult = Parser::parseConfig(config);
    std::string interval = parseResult.at(Parser::INTVAL);
    intval = static_cast<unsigned int>(std::stol(interval));
    input = homedir + "/" + parseResult.at(Parser::IN_DIR);
    output = homedir + "/" + parseResult.at(Parser::OUT_DIR);
    extension = parseResult.at(Parser::EXTENSION);
}

bool Daemon::initPid() {
    if (setsid() == -1) {
        throw std::runtime_error("setsid error");
    }
    pid_t pidThread = fork();
    if (pidThread == -1) {
        throw std::runtime_error("fork failed");
    }
    if (pidThread != 0) {
        return false;
    }
    umask(0);
    if (chdir("/") == -1) {
        throw std::runtime_error("chdir error");
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    CheckPid();
    return true;
}

void Daemon::CheckPid() {
    std::ifstream pidFile(pid);
    if (pidFile.is_open()) {
        pid_t other;
        pidFile >> other;
        pidFile.close();
        struct stat sb;
        std::string path = "/proc/" + std::to_string(other);
        if (stat(path.c_str(), &sb) == 0) {
            kill(other, SIGTERM);
        }
    }
    SavePid();
}

void Daemon::SavePid() {
    std::ofstream out(pid);
    if (!out) {
        throw std::runtime_error("Can't open pid file");
    }
    out << getpid();
    out.close();
    syslog(LOG_NOTICE, "Thread init complete");
}

void Daemon::cleanDst() {
    syslog(LOG_INFO, "Start clean dir");
    if (nftw(output.c_str(), removeFile, 10, FTW_DEPTH | FTW_MOUNT | FTW_PHYS) < 0) {
        throw std::runtime_error("Cannot remove file");
    }
    if (mkdir(output.c_str(), 0777) == -1) {
        throw std::runtime_error("Cannot create directory");
    }
}

int Daemon::removeFile(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb) {
    if (remove(pathname) < 0) {
        syslog(LOG_ERR, "Can't remove file: %s", pathname);
        return -1;
    }
    syslog(LOG_INFO, "Complete removing file %s", pathname);
    return 0;
}

int Daemon::copyFile(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb) {
    std::string file = pathname;
    std::string::size_type index = file.rfind('.');
    std::string::size_type indexName = file.rfind('/');
    std::string dstFile;
    if (indexName == std::string::npos) {
        dstFile = file;
    } else {
        dstFile = file.substr(indexName + 1);
    }
    if (index != std::string::npos) {
        std::string ext = file.substr(index + 1);
        if (ext == extension) {
            syslog(LOG_INFO, "Copy file: %s", pathname);
            std::ifstream src(pathname, std::ios::binary);
            std::string dstPath = output + dstFile;
            std::ofstream dst(dstPath, std::ios::binary);
            dst << src.rdbuf();
            src.close();
            dst.close();
        }
    }
    return 0;
}

void Daemon::copySrcToDst() {
    nftw(input.c_str(), copyFile, 10, FTW_DEPTH | FTW_MOUNT | FTW_PHYS);
}

bool Daemon::initTread() {
    syslog(LOG_INFO, "Start init thread");
    pid_t pid_t = fork();
    if (pid_t == -1) {
        throw std::runtime_error("Fork failed");
    } else if (pid_t == 0) {
        return initPid();
    }
    return false;
}



