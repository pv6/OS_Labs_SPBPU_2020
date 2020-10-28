//
// Created by Evgenia on 14.10.2020.
//

#ifndef INC_1_DAEMON_H
#define INC_1_DAEMON_H

#include "directoryUtils.h"

class Daemon
{
public:

    static const std::string pid_path;

    static Daemon& GetDaemonInst();
    static void SignalHandler(int signal);
    void SetConfig(const std::string& configPath);

    void ReadConfig();
    void Run();
    void Terminate();

private:
    Daemon();

    Daemon(const Daemon &) = delete;
    Daemon &operator=(const Daemon &) = delete;

    bool is_running = false;
    bool is_terminated = false;
    std::string config_path;
    std::string path_from;
    std::string path_to;
    int copy_interval;
    int new_file_time_limit;

    static void SavePid();
    static void CheckPrevPid();

    void CopyFiles();

};

#endif //INC_1_DAEMON_H