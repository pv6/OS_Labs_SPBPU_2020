#include "../include/daemon.h"

#include <stdlib.h> // realpath
#include <fcntl.h>  // open
#include <unistd.h> // fork, setsid, dup, close, getpid
#include <syslog.h> // syslog
#include <signal.h> // signal, kill
#include <fstream>  // ifstream, ofstream

#include <sys/types.h> 
#include <sys/stat.h> // umask

Daemon::Daemon(std::string const& config_path, std::string const& pid_path) {
    // save absolute paths
	char* buff = new char[256];
	if (realpath(config_path.c_str(), buff) == NULL) {
        delete[] buff;
        throw InvalidPathException();
    }
	m_config_path = std::string(buff);
	if (system((std::string("touch ") + pid_path).c_str()) != 0 ||
        realpath(pid_path.c_str(), buff) == NULL) {
        delete[] buff;
        throw InvalidPathException();
    }
	m_pid_path = std::string(buff);
	delete[] buff;
}

bool Daemon::daemonize(void (*handle_signal)(int)) const {
    // 1st fork
    int pid = (int)fork();
    if (pid < 0) {
        throw ForkException();
    }
    if (pid > 0) {
        return false;
    }

    // set session id
    if (setsid() < 0) {
        throw SetSessionIDException();
    }

    // 2nd fork
    pid = (int)fork();
    if (pid < 0) {
        throw ForkException();
    }
    if (pid > 0) {
        return false;
    }

    // close all "inherited" opened files
    for (int i = sysconf(_SC_OPEN_MAX); i >=0; --i) {
        close(i);
    }
    syslog(LOG_DEBUG, "files from dtable have been closed");

    // re-open standard streams and redirect to /dev/null for safety
    int std_fid = open("/dev/null", O_RDWR);
    if (std_fid == -1) {
        throw OpenFileException();
    }
    if (dup(std_fid) == -1 || dup(std_fid) == -1) {
        throw DuplicateFidException();
    }  
    syslog(LOG_DEBUG, "standard streams have been opened");

    // reset the umask
    umask(0);

    // change working directory of process in order to avoid blocking mount points 
    if (chdir("/") == -1) {
        throw ChangeDirException();
    }
    syslog(LOG_DEBUG, "working directory has been changed");

    // add signal handlers
    if (signal(SIGHUP, (sighandler_t)handle_signal) == SIG_ERR ||
        signal(SIGTERM, (sighandler_t)handle_signal) == SIG_ERR) {
        throw SignalException();
    }
    syslog(LOG_DEBUG, "signal handlers have been established");
    
    // check whether another instance of program is currently running, terminate it
    std::ifstream pid_ifstream;
    pid_ifstream.open(m_pid_path.c_str(), std::ifstream::in);
    if (pid_ifstream.good()) {
        int existing_pid;
        if (!(pid_ifstream >> existing_pid)) {
            syslog(LOG_WARNING, "pid file might be corrupted");
        }
        else if (kill(existing_pid, 0) == 0) {
            syslog(LOG_NOTICE, "another instance of running program has been found");
            kill(existing_pid, SIGTERM);
            syslog(LOG_NOTICE, "another instance of running program has been terminated");
        }
        pid_ifstream.close();
    }

    // save pid to .pid file
    std::ofstream pid_ofstream;
    pid_ofstream.open(m_pid_path.c_str(), std::ofstream::out);
    if (pid_ofstream.good()) {
        pid_ofstream << (int)getpid();
        pid_ofstream.close();
        syslog(LOG_INFO, "pid has been saved to file");
    }
    else {
        throw OpenFileException();
    }

    // success
    syslog(LOG_INFO, "process successfully daemonized");
    return true;
}
