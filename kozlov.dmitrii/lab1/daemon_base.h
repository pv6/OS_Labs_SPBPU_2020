#pragma once

#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <syslog.h>
#include <csignal>
#include <string>
#include <functional>
#include "cfg_parser.h"
#include "constants.h"
#include "types.h"


class DaemonBase{
public:
    ~DaemonBase();
    template<typename T>
    static PROCCESS_TYPE CreateDaemon(string &, std::shared_ptr<DaemonBase> &) noexcept(false);
    CfgParser * GetParser() {return _pConfig.get();}
    virtual void SetSignalHandler(void (*_pSignalHandler)(int));
    virtual void process() = 0;
    virtual void finish() {_isFinished = true;}
private:
    static PROCCESS_TYPE Fork() noexcept(false);
    void KillSiblings() noexcept(false);
    void SetPid() noexcept(false);
protected:
    DaemonBase() = default;
protected:
    CfgParserPtr _pConfig;
    bool _isFinished = false;
};
using DaemonBasePtr = std::shared_ptr<DaemonBase>;

template <typename T>
PROCCESS_TYPE DaemonBase::CreateDaemon(string & cfg, std::shared_ptr<DaemonBase> & pDaemon)
{
    openlog("MyDaemon", LOG_PID, LOG_DAEMON);
    if (Fork() == PROCCESS_TYPE::PARENT)
        return PROCCESS_TYPE::PARENT;

    if (setsid() < 0)
        throw std::runtime_error("Can't create the new session");

    if (Fork() == PROCCESS_TYPE::PARENT)
        return PROCCESS_TYPE::PARENT;

    std::stringstream ss;
    ss << get_current_dir_name() << "/" << cfg;
    string absCfgPath = ss.str();

    umask(0);
    if ((chdir("/")) < 0)
        throw std::runtime_error("Can't change directory");

    long num = sysconf(_SC_OPEN_MAX);
    while (num >= 0)
        close(num--);

    pDaemon = std::shared_ptr<T>(new T());
    if (!pDaemon.get())
        throw std::runtime_error("Can't create daemon instance");

    pDaemon->KillSiblings();
    pDaemon->SetPid();

    pDaemon->_pConfig = std::make_unique<CfgParser>(absCfgPath);
    syslog(LOG_INFO, "Daemon was linked with parser");
    pDaemon->_pConfig->ReadConfig();

    syslog(LOG_INFO, "Daemon was created successfully");
    return PROCCESS_TYPE::CHILD;
}

