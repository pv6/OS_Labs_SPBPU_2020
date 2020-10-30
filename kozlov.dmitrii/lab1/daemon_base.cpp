#include "daemon_base.h"

DaemonBase::~DaemonBase()
{
    syslog(LOG_INFO, "Daemon was destructed");
    closelog();
}

PROCCESS_TYPE DaemonBase::Fork() {
    switch (fork()) {
        case -1:
            throw std::runtime_error("Can't fork the process");
        case 0:
            return PROCCESS_TYPE::CHILD;
        default:
            closelog();
            return PROCCESS_TYPE::PARENT;
    }
}

void DaemonBase::KillSiblings()
{
    std::ifstream pidStream;
    pidStream.open(CONSTANTS::PID_PATH);
    if (!pidStream.is_open())
        throw std::ifstream::failure("Can't open PID file on read");
    pid_t prev;
    pidStream >> prev;
    if (prev > 0) {
        kill(prev, SIGTERM);
    }
    pidStream.close();
    syslog(LOG_INFO, "Daemon's siblings was killed");
}

void DaemonBase::SetPid()
{
    std::ofstream pidStream(CONSTANTS::PID_PATH, std::ios_base::out | std::ios_base::trunc);
    if (!pidStream.is_open())
        throw std::ifstream::failure("Can't open PID file on write");
    pidStream << getpid();
    pidStream.close();
    syslog(LOG_INFO, "Daemon PID was written to file");
}

void DaemonBase::SetSignalHandler(void (*/*_pSignalHandler*/)(int)) {
    signal(SIGHUP, SIG_DFL);
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);

    signal(SIGILL, SIG_DFL);
    signal(SIGABRT, SIG_DFL);
    signal(SIGTRAP, SIG_DFL);

    signal(SIGIOT, SIG_DFL);
    signal(SIGFPE, SIG_DFL);
    signal(SIGKILL, SIG_DFL);

    signal(SIGBUS, SIG_DFL);
    signal(SIGSEGV, SIG_DFL);
    signal(SIGXCPU, SIG_DFL);

    signal(SIGSYS, SIG_DFL);
    signal(SIGPIPE, SIG_DFL);
    signal(SIGALRM, SIG_DFL);

    signal(SIGTERM, SIG_DFL);
    signal(SIGUSR1, SIG_DFL);
    signal(SIGUSR2, SIG_DFL);

    signal(SIGCHLD, SIG_DFL);
    signal(SIGCLD, SIG_DFL);
    signal(SIGPWR, SIG_DFL);

    syslog(LOG_INFO, "Signal Handlers was updated");
}



