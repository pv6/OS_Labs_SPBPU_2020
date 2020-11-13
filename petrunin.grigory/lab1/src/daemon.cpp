#include "../include/daemon.h"
#include "../include/exceptions.h"

#include <stdlib.h> // realpath
#include <fcntl.h>  // open
#include <unistd.h> // fork, setsid, dup, close, getpid
#include <syslog.h> // syslog
#include <signal.h> // signal, kill
#include <fstream>  // ifstream, ofstream

#include <sys/types.h> 
#include <sys/stat.h> // umask
#include <limits.h>   // PATH_MAX

Daemon* Daemon::m_instance = nullptr;

Daemon* Daemon::getDaemon(std::string const& path_pidf) {
    if (m_instance != nullptr) {
        return m_instance;
    }

    // get absolute path
	char* buff = new char[PATH_MAX];
	if (realpath(path_pidf.c_str(), buff) == NULL) {
        delete[] buff;
        throw InvalidPathException();
    }
    m_instance = new Daemon(std::string(buff));
	delete[] buff;
    return m_instance;
}

Daemon::Daemon(std::string const& path_pidf) :
    m_path_pidf(path_pidf) {}

bool Daemon::daemonize() const {
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
    if (signal(SIGHUP, (sighandler_t)Daemon::handle_signal) == SIG_ERR ||
        signal(SIGTERM, (sighandler_t)Daemon::handle_signal) == SIG_ERR) {
        throw SignalException();
    }
    syslog(LOG_DEBUG, "signal handlers have been established");
    
    // check whether another instance of program is currently running, terminate it
    std::ifstream pid_ifstream;
    pid_ifstream.open(m_path_pidf.c_str(), std::ifstream::in);
    if (pid_ifstream.good()) {
        int existing_pid;
        // TODO: there's smth wrong
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
    pid_ofstream.open(m_path_pidf.c_str(), std::ofstream::out);
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

void Daemon::handle_signal(int signal) {
    switch(signal) {
    case SIGHUP:
        syslog(LOG_INFO, "SIGHUP signal catched");
        if (Daemon::m_instance != nullptr) {
            Daemon::m_instance->worker_reconfigure();
        }
        break;
    case SIGTERM:
        syslog(LOG_INFO, "SIGTERM signal catched");
        if (Daemon::m_instance != nullptr) {
            Daemon::m_instance->worker_stop();
        }
    }
}

void Daemon::worker_set(Worker* worker) {
    if (worker == nullptr) {
        throw NullPtrException();
    }
    m_worker = worker;
}

void Daemon::worker_unset() {
    m_worker = nullptr;
}

void Daemon::worker_run() {
    if (m_worker != nullptr) {
        syslog(LOG_DEBUG, "running worker");
        m_worker->work();
    }
}

void Daemon::worker_reconfigure() {
    if (m_worker != nullptr) {
        syslog(LOG_DEBUG, "reconfiguring worker");
        m_worker->reconfigure();
    }
}

void Daemon::worker_stop() {
    if (m_worker != nullptr) {
        syslog(LOG_DEBUG, "stopping worker");
        m_worker->stop();
    }
}
